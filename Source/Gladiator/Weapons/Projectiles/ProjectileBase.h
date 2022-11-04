// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UProjectileMovementComponent;

UCLASS()
class GLADIATOR_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

	// Intializes the velocity of the weapon.
	void InitVelocity(const FVector& Direction);

protected:

	// Replication setup
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile base")
	UProjectileMovementComponent* ProjectileMovement;

	// What is the initial velocity of this weapon?
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_InitialVelocity, Category = "Projectile Base")
	FVector InitialVelocity;

	UFUNCTION()
	void OnRep_InitialVelocity();
};
