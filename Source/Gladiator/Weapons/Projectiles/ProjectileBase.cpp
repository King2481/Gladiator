// Created by Bruce Crum


#include "Gladiator/Weapons/Projectiles/ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ShapeComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	bReplicates = true;
}

void AProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AProjectileBase, InitialVelocity, COND_InitialOnly);
}

void AProjectileBase::InitVelocity(const FVector& Direction)
{
	if (ProjectileMovement)
	{
		InitialVelocity = Direction * ProjectileMovement->InitialSpeed;
	}
}

void AProjectileBase::OnRep_InitialVelocity()
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = InitialVelocity;
	}
}
