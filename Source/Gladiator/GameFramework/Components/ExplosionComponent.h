// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExplosionComponent.generated.h"

class UCameraShakeBase;
class USoundCue;

USTRUCT(BlueprintType)
struct FExplosionConfig
{
	GENERATED_BODY()

	///////////////////////////////////////////////////////////////////
	// Explosion Config

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ExplosionInnerRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ExplosionOuterRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ExplosionInnerDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ExplosionOuterDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UDamageType> ExplosionDamageTypeClass;

	///////////////////////////////////////////////////////////////////
	// Camera Shake Config

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCameraShakeBase> ExplosionCameraShakeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ExplosionCameraShakeInnerRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ExplosionCameraShakeOuterRadius;

	//////////////////////////////////////////////////////////////////
	// FX

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USoundCue* ExplosionSound;

	FExplosionConfig()
	{
		ExplosionInnerRadius = 150.0f;
		ExplosionOuterRadius = 450.0f;
		ExplosionInnerDamage = 100.0f;
		ExplosionOuterDamage = 10.0f;
		ExplosionDamageTypeClass = nullptr;

		//ExplosionCameraShakeClass = nullptr;
		ExplosionCameraShakeInnerRadius = 200.0f;
		ExplosionCameraShakeOuterRadius = 1000.0f;

		ExplosionSound = nullptr;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLADIATOR_API UExplosionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UExplosionComponent();

	// The explosion config for this component;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Explosion Component")
	FExplosionConfig ExplosionConfig;

	// Will "explode" and deal radial damage based on the owning actors location.
	UFUNCTION(BlueprintCallable, Category = "Explosion Component")
	void Explode();

	// Will "explode" and deal radial damage based on the owning actors location, allows for specific and other explosion configs.
	UFUNCTION(BlueprintCallable, Category = "Explosion Component")
	void ExplodeAt(const FVector& Location, const FExplosionConfig& InExplosionConfig);

protected:

	// Explosion Multicast, plays things like FX and sound.
	UFUNCTION(NetMulticast, Reliable)
	void MulticastExplode(const FVector& Location, const FExplosionConfig& InExplosionConfig);
	void MulticastExplode_Implementation(const FVector& Location, const FExplosionConfig& InExplosionConfig);
};
