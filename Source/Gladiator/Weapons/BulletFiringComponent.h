// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletFiringComponent.generated.h"

class AProjectileBase;
class UAmmoComponent;
class UGladiatorDamageType;

DECLARE_LOG_CATEGORY_EXTERN(LogBulletFiring, Log, Verbose);

USTRUCT(BlueprintType)
struct FStoredFirearmHit
{
	GENERATED_BODY()

	// The actor that we had hit.
	UPROPERTY()
	AActor* HitActor;

	// The physical material that we hit.
	UPROPERTY()
	TWeakObjectPtr<UPhysicalMaterial> PhysMaterial;

	// Origin of the shot
	UPROPERTY()
	FVector_NetQuantize Origin;

	// Where the shot landed
	UPROPERTY()
	FVector_NetQuantize ImpactPoint;

	// The impact normal of the hit
	UPROPERTY()
	FVector_NetQuantizeNormal ImpactNormal;

	// The direction of the shot
	UPROPERTY()
	FVector_NetQuantize ShotDirection;

	FStoredFirearmHit()
	{
		HitActor = nullptr;
		PhysMaterial = nullptr;
		Origin = FVector_NetQuantize();
		ImpactPoint = FVector_NetQuantize();
		ImpactNormal = FVector_NetQuantize();
		ShotDirection = FVector_NetQuantize();
	}

	FStoredFirearmHit(AActor* StoredActor, TWeakObjectPtr<UPhysicalMaterial> StoredMaterial, const FVector& StoredOrigin, const FVector& StoredImpactPoint, const FVector& StoredImpactNormal, const FVector& StoredShotDirection)
	{
		HitActor = StoredActor;
		PhysMaterial = StoredMaterial;
		Origin = StoredOrigin;
		ImpactPoint = StoredImpactPoint;
		ImpactNormal = StoredImpactNormal;
		ShotDirection = StoredShotDirection;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLADIATOR_API UBulletFiringComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBulletFiringComponent();

	UFUNCTION(BlueprintCallable, Category = "Bullet Firing Component")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Bullet Firing Component")
	void EndFire();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	bool AllowFire() const;

	void FireBullets();

	void FireProjectile();

	// Server RPC for firing a projectile.
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireProjectile(const FVector_NetQuantize& Location, const TArray<FVector_NetQuantizeNormal>& Directions);
	void ServerFireProjectile_Implementation(const FVector_NetQuantize& Location, const TArray<FVector_NetQuantizeNormal>& Directions);
	bool ServerFireProjectile_Validate(const FVector_NetQuantize& Location, const TArray<FVector_NetQuantizeNormal>& Directions);

	// Actual trace for the firearm
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const;

	// Process whatever we just hit.
	void ProcessInstantHits(const TArray<FStoredFirearmHit>& Hits);

	bool bWantsToFire;

	// If this setting fires an actual projectile, what is it?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	TSubclassOf<AProjectileBase> ProjectileClass;

	// If hitscan, how far does the hitscan go?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	float HitscanDistance;

	// How many bullets are fired each time we fire?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	uint8 BulletsPerShot;

	// What is the damage model for this weapon?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	float Damage;

	// What type of damage is applied to characters who are hit?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	TSubclassOf<UDamageType> DamageTypeClass;

	// What is the Min/Max spread of this weapon? X represents min and Y represents max
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet Firing Component")
	FVector2D SpreadData;

	// How much ammo is depeleted per shot? (Only valid if ammo component is valid.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	int32 AmmoPerShot;

	// In rounds per minute, how quickly does this component fire?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	float RateOfFire;

	// When we pull the trigger, is there a delay when the bullet actually fires? If negative number trigger delay is not applied.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	float InitialTriggerDelay;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	FName MuzzleSocketName;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Bullet Firing Component")
	UAmmoComponent* LinkedAmmoComponent;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Bullet Firing Component")
	USkeletalMeshComponent* LinkedMeshComponent;

	// When was the last time we fired this component?
	UPROPERTY(BlueprintReadOnly, Category = "Bullet Firing Component")
	float LastFireTime;

	// When are we allowed to fire this component next?
	UPROPERTY(BlueprintReadOnly, Category = "Bullet Firing Component")
	float NextFireTime;

	void GetOwnerViewLocation(FVector& OutEyesLocation, FRotator& OutEyesRotation);

	FVector GetMuzzleLocation();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	FVector MuzzleOffset;

	// Maping for damage multiplier, use this if you want specific bonuses/penalties to apply when a bullet does damage against something.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Bullet Firing Component")
	TMap<UPhysicalMaterial*, float> DamageMultiplierMap;

public:	

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// What is the rate of fire for this weapon? (Returns it as a fraction)
	UFUNCTION(BlueprintPure, Category = "Bullet Firing Component")
	float GetRateOfFireAsFraction() const;	

	// Is the owner of this component locally controlled?
	UFUNCTION(BlueprintPure, Category = "Bullet Firing Component")
	bool IsOwnerLocallyControlled() const;

private:

	// Notify the server that we hit something and needs validation.
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNotifyHit(const TArray<FStoredFirearmHit>& Hits);
	void ServerNotifyHit_Implementation(const TArray<FStoredFirearmHit>& Hits);
	bool ServerNotifyHit_Validate(const TArray<FStoredFirearmHit>& Hits);

	// Called when a hit was confirmed. (this uses the whole stored hits and assume all are valid)
	void ConfirmedFirearmHits(const TArray<FStoredFirearmHit>& Hits);

	// Called when a hit was confirmed.
	void ConfirmedFirearmHit(const FStoredFirearmHit& Hit);

	// Validates a possible firearm hit
	bool ValidateFirearmHit(const FStoredFirearmHit& Hit);

	// Checks to see if we should actually deal damage.
	bool ShouldDealDamage(AActor* TestActor) const;

};
