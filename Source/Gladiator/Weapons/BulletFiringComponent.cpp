// Created by Bruce Crum


#include "Gladiator/Weapons/BulletFiringComponent.h"
#include "Gladiator/Weapons/Ammo/AmmoComponent.h"
#include "Gladiator/Engine/GameTraceChannels.h"
#include "Gladiator/Weapons/WeaponGameplayStatics.h"
#include "Gladiator/Weapons/Projectiles/ProjectileBase.h" // TODO: See if we can get rid of this header.

#if !UE_BUILD_SHIPPING
static TAutoConsoleVariable<int32> CvarShowWeaponTraces(TEXT("DebugWeaponTraces"), 0, TEXT("Visualise Firearm Traces"));
static TAutoConsoleVariable<int32> CvarInfiniteAmmo(TEXT("InfiniteAmmo"), 0, TEXT("Grants Infinite Ammo based on thread value (1 = no bullets are decremented. 2 = ammo checks for firing are ignored)"));

#include "Kismet/KismetSystemLibrary.h"
#endif

DEFINE_LOG_CATEGORY(LogBulletFiring);

#define MAX_DISTANCE_FROM_ORIGIN 5000.0f

// Sets default values for this component's properties
UBulletFiringComponent::UBulletFiringComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);

	bWantsToFire = false;
	LastFireTime = -1.0f;
	NextFireTime = -1.0f;

	ProjectileClass = nullptr;
	HitscanDistance = 20000.0f;
	BulletsPerShot = 1;
	Damage = 20.0f;
	DamageTypeClass = nullptr;
	RateOfFire = 750.0f;
	InitialTriggerDelay = -1.0f;
	MuzzleSocketName = FName("MuzzleSocket");
	SpreadData = FVector2D::ZeroVector;
	MuzzleOffset = FVector::ZeroVector;
	AmmoPerShot = 1;
}

// Called when the game starts
void UBulletFiringComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
}

// Called every frame
void UBulletFiringComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bWantsToFire && IsOwnerLocallyControlled() && AllowFire())
	{
		FireBullets();
	}
}

void UBulletFiringComponent::StartFire()
{
	bWantsToFire = true;

	if (InitialTriggerDelay > FLT_EPSILON)
	{
		NextFireTime = GetWorld()->TimeSeconds + InitialTriggerDelay;
	}

	if (AllowFire())
	{
		FireBullets();
	}

	SetComponentTickEnabled(true);
}

void UBulletFiringComponent::EndFire()
{
	bWantsToFire = false;

	SetComponentTickEnabled(false);
}

void UBulletFiringComponent::FireBullets()
{
	if (ProjectileClass)
	{
		FireProjectile();
	}
	else
	{
		FVector StartTrace;
		FRotator AimRot;

		TArray<FStoredFirearmHit> StoredHits;

		GetOwnerViewLocation(StartTrace, AimRot);

		const FVector AimDir = AimRot.Vector();

		for (uint8 i = 0; i < BulletsPerShot; i++)
		{
			const int32 RandomSeed = FMath::Rand();
			FRandomStream WeaponRandomStream(RandomSeed);
			const float CurrentSpread = 1.f;
			const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * FMath::RandRange(SpreadData.X, SpreadData.Y));

			const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
			const FVector EndTrace = StartTrace + (ShootDir * HitscanDistance);

			const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
			StoredHits.Add(FStoredFirearmHit(Impact.GetActor(), Impact.PhysMaterial, StartTrace, Impact.ImpactPoint, Impact.ImpactNormal, ShootDir));
		}

		ProcessInstantHits(StoredHits);
	}

	LastFireTime = GetWorld()->TimeSeconds;
	NextFireTime = GetWorld()->TimeSeconds + GetRateOfFireAsFraction();

	bool bDecrementAmmo = true;

#if !UE_BUILD_SHIPPING
	if (CvarInfiniteAmmo.GetValueOnGameThread() > 0)
	{
		bDecrementAmmo = false;
	}
#endif
	if (bDecrementAmmo && LinkedAmmoComponent)
	{
		LinkedAmmoComponent->ModifyAmmo(-AmmoPerShot);
	}
}

FHitResult UBulletFiringComponent::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(GetOwner()->GetOwner());
	Params.bTraceComplex = false;
	Params.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Bullet, Params);

#if !UE_BUILD_SHIPPING
	if (CvarShowWeaponTraces.GetValueOnGameThread() > 0)
	{
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartTrace, EndTrace, FLinearColor::Red, 1.0f, 1.0f);

		if (Hit.GetActor())
		{
			UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.0f, 12, FLinearColor::Yellow, 1.0f, 1.0f);
		}
	}
#endif

	return Hit;
}

void UBulletFiringComponent::ProcessInstantHits(const TArray<FStoredFirearmHit>& Hits)
{
	if (IsOwnerLocallyControlled() && GetNetMode() == NM_Client)
	{
		// notify the server of the hit
		ServerNotifyHit(Hits);
		return;
	}

	// Not a client, go ahead and confirm the hit.
	ConfirmedFirearmHits(Hits);
}

void UBulletFiringComponent::ServerNotifyHit_Implementation(const TArray<FStoredFirearmHit>& Hits)
{
	for (auto& Hit : Hits)
	{
		if (ValidateFirearmHit(Hit))
		{
			ConfirmedFirearmHit(Hit);
		}
	}
}

bool UBulletFiringComponent::ServerNotifyHit_Validate(const TArray<FStoredFirearmHit>& Hits)
{
	return true;
}

bool UBulletFiringComponent::ValidateFirearmHit(const FStoredFirearmHit& Hit)
{
	if (!GetOwner())
	{
		UE_LOG(LogBulletFiring, Warning, TEXT("%s Rejected client side hit of %s (No Owner)"), *GetNameSafe(this), *GetNameSafe(Hit.HitActor));
		return false;
	}

	const float DistanceFromHit = (Hit.Origin - Hit.ImpactPoint).Size();
	if (DistanceFromHit > HitscanDistance)
	{
		UE_LOG(LogBulletFiring, Warning, TEXT("%s Rejected client side hit of %s (Out of range)"), *GetNameSafe(this), *GetNameSafe(Hit.HitActor));
		return false;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	GetOwnerViewLocation(ViewLocation, ViewRotation);

	const float DistanceFromPlayer = (Hit.Origin - ViewLocation).Size();
	if (DistanceFromPlayer > MAX_DISTANCE_FROM_ORIGIN)
	{
		UE_LOG(LogBulletFiring, Warning, TEXT("%s Rejected client side hit of %s (Shot origin too far from player)"), *GetNameSafe(this), *GetNameSafe(Hit.HitActor));
		return false;
	}

	return true;
}

void UBulletFiringComponent::ConfirmedFirearmHits(const TArray<FStoredFirearmHit>& Hits)
{
	for (auto& Hit : Hits)
	{
		ConfirmedFirearmHit(Hit);
	}
}

void UBulletFiringComponent::ConfirmedFirearmHit(const FStoredFirearmHit& Hit)
{
	// handle damage
	if (ShouldDealDamage(Hit.HitActor))
	{
		FHitResult Impact;
		Impact.PhysMaterial = Hit.PhysMaterial;
		Impact.ImpactPoint = Hit.ImpactPoint;
		Impact.TraceStart = Hit.Origin;
		Impact.ImpactNormal = Hit.ImpactNormal;

		float Multiplier = 1.0f;

		if (Hit.PhysMaterial.IsValid() && DamageMultiplierMap.Contains(Hit.PhysMaterial.Get()))
		{
			Multiplier *= DamageMultiplierMap.FindRef(Hit.PhysMaterial.Get());
		}

		const float AppliedDamage = Damage * Multiplier;
		
		UGameplayStatics::ApplyDamage(Hit.HitActor, AppliedDamage, GetOwner()->GetInstigatorController(), GetOwner(), DamageTypeClass);
	}
}

bool UBulletFiringComponent::ShouldDealDamage(AActor* TestActor) const
{
	// if we're an actor on the server, or the actor's role is authoritative, we should register damage
	if (TestActor)
	{
		if (GetNetMode() != NM_Client ||
			TestActor->GetLocalRole() == ROLE_Authority ||
			TestActor->GetTearOff())
		{
			return true;
		}
	}

	return false;
}

void UBulletFiringComponent::FireProjectile()
{
	FVector EyesLocation;
	FRotator AimRot;
	GetOwnerViewLocation(EyesLocation, AimRot);
	
	const FVector AimDir = AimRot.Vector();

	TArray<FVector_NetQuantizeNormal> Directions; // Send fire directions in a single payload so that way we're not constantly sending server RPC calls.

	for (uint8 i = 0; i < BulletsPerShot; i++)
	{
		const int32 RandomSeed = FMath::Rand();
		FRandomStream WeaponRandomStream(RandomSeed);
		const float ConeHalfAngle = FMath::DegreesToRadians(1.f * FMath::RandRange(SpreadData.X, SpreadData.Y));
		Directions.Add(WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle));
	}

	ServerFireProjectile(GetMuzzleLocation(), Directions);
}

void UBulletFiringComponent::ServerFireProjectile_Implementation(const FVector_NetQuantize& Location, const TArray<FVector_NetQuantizeNormal>& Directions)
{
	AActor* TopOwner = GetOwner()->GetOwner() ? GetOwner()->GetOwner() : GetOwner();

	for (auto& ShootDir : Directions)
	{
		UWeaponGameplayStatics::FireProjectile(this, TopOwner, ProjectileClass, Location, ShootDir);
	}
}

bool UBulletFiringComponent::ServerFireProjectile_Validate(const FVector_NetQuantize& Location, const TArray<FVector_NetQuantizeNormal>& Directions)
{
	return true;
}

bool UBulletFiringComponent::AllowFire() const
{
	if (!GetWorld() || GetWorld()->TimeSeconds < NextFireTime)
	{
		// Not a valid world or not allowed to fire yet.
		return false;
	}

	bool bCheckAmmo = true;

#if !UE_BUILD_SHIPPING
	if (CvarInfiniteAmmo.GetValueOnGameThread() > 1)
	{
		bCheckAmmo = false;
	}
#endif

	if (bCheckAmmo && LinkedAmmoComponent && LinkedAmmoComponent->GetAmmo() < AmmoPerShot)
	{
		return false;
	}

	return true;
}

float UBulletFiringComponent::GetRateOfFireAsFraction() const
{
	return 60.0f / RateOfFire;
}

bool UBulletFiringComponent::IsOwnerLocallyControlled() const
{
	return GetOwner()->HasLocalNetOwner();
}

void UBulletFiringComponent::GetOwnerViewLocation(FVector& OutEyesLocation, FRotator& OutEyesRotation)
{
	// Does the owner have an owner? (In the case of a firearm having an owner)
	if (GetOwner()->GetOwner())
	{
		GetOwner()->GetOwner()->GetActorEyesViewPoint(OutEyesLocation, OutEyesRotation);
	}
	else
	{
		GetOwner()->GetActorEyesViewPoint(OutEyesLocation, OutEyesRotation);
	}
}

FVector UBulletFiringComponent::GetMuzzleLocation()
{
	if (LinkedMeshComponent)
	{
		return LinkedMeshComponent->GetSocketLocation(MuzzleSocketName) + MuzzleOffset;
	}
	else
	{
		// Default to eyes location if we don;t have a linked mesh.
		FVector EyesLocation;
		FRotator EyesRotation;
		GetOwnerViewLocation(EyesLocation, EyesRotation);
	
		return EyesLocation;
	}
}

