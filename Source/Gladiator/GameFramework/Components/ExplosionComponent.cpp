// Created by Bruce Crum


#include "Gladiator/GameFramework/Components/ExplosionComponent.h"
#include "Kismet/GameplayStatics.h"

#if !UE_BUILD_SHIPPING
static TAutoConsoleVariable<int32> CvarDebugExplosion(TEXT("DebugExplosions"), 0, TEXT("Visualise Explosion Info"));

#include "Kismet/KismetSystemLibrary.h" 
#endif

// Sets default values for this component's properties
UExplosionComponent::UExplosionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	// ...
}

void UExplosionComponent::Explode()
{
	ExplodeAt(GetOwner()->GetActorLocation(), ExplosionConfig);
}

void UExplosionComponent::ExplodeAt(const FVector& Location, const FExplosionConfig& InExplosionConfig)
{
	if (GetOwnerRole() >= ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(GetOwner());

		UGameplayStatics::ApplyRadialDamageWithFalloff(this, 
			InExplosionConfig.ExplosionInnerDamage, 
			InExplosionConfig.ExplosionOuterDamage, 
			Location + FVector(0.0f, 0.0f, 1.0f), 
			InExplosionConfig.ExplosionInnerRadius, 
			InExplosionConfig.ExplosionOuterRadius, 
			1.0f, 
			InExplosionConfig.ExplosionDamageTypeClass, 
			IgnoredActors, 
			GetOwner(), 
			GetOwner()->GetInstigatorController());

		if (ExplosionConfig.ExplosionCameraShakeClass)
		{
			UGameplayStatics::PlayWorldCameraShake(this, InExplosionConfig.ExplosionCameraShakeClass, Location, InExplosionConfig.ExplosionCameraShakeInnerRadius, InExplosionConfig.ExplosionCameraShakeOuterRadius);
		}

		MulticastExplode(Location, InExplosionConfig);
	}
}

void UExplosionComponent::MulticastExplode_Implementation(const FVector& Location, const FExplosionConfig& InExplosionConfig)
{
#if !UE_SERVER

	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		// Don't play on dedicated.
		return;
	}

	/*if (InExplosionConfig.ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetOwner(), InExplosionConfig.ExplosionSound, Location);
	}

	if (InExplosionConfig.ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), InExplosionConfig.ExplosionFX, Location);
	}*/

#if !UE_BUILD_SHIPPING
	if (CvarDebugExplosion.GetValueOnGameThread() > 0)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location + FVector(0.0f, 0.0f, 1.0f), InExplosionConfig.ExplosionInnerRadius, 12, FColor::Red, 5.0f, 1.0f);
		UKismetSystemLibrary::DrawDebugSphere(this, Location + FVector(0.0f, 0.0f, 1.0f), InExplosionConfig.ExplosionOuterRadius, 12, FColor::Yellow, 5.0f, 1.0f);
	}
#endif

#endif	
}