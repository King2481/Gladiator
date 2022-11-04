// Created by Bruce Crum


#include "Gladiator/Weapons/WeaponGameplayStatics.h"
#include "Gladiator/Weapons/Projectiles/ProjectileBase.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY(LogWeaponGameplayStatics);

AProjectileBase* UWeaponGameplayStatics::FireProjectile(const UObject* WorldContextObject, AActor* Instigator, TSubclassOf<AProjectileBase> ProjectileClass, const FVector& Location, const FVector& ShootDir)
{
	const auto World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	if (!World)
	{
		UE_LOG(LogWeaponGameplayStatics, Error, TEXT("Error: UWeaponGameplayStatics::FireProjectile() failed (Invalid World)"));
	}

	if (!ProjectileClass)
	{
		UE_LOG(LogWeaponGameplayStatics, Error, TEXT("Error: UWeaponGameplayStatics::FireProjectile() failed (Projectile Class Invalid)"));
		return nullptr;
	}

	const FTransform SpawnTransform = FTransform(ShootDir.Rotation(), Location);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(Instigator);
	SpawnParams.Owner = Instigator;

	AProjectileBase* Projectile = World->SpawnActor<AProjectileBase>(ProjectileClass, SpawnTransform, SpawnParams);
	if (Projectile)
	{
		Projectile->InitVelocity(ShootDir);
		return Projectile;
	}

	UE_LOG(LogWeaponGameplayStatics, Error, TEXT("Error: UWeaponGameplayStatics::FireProjectile(), failed to fire projectile %s (Failed to spawn)"), *ProjectileClass->GetName());
	return nullptr;
}