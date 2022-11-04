// Created by Bruce Crum


#include "Gladiator/Weapons/Ammo/AmmoComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UAmmoComponent::UAmmoComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	AmmoType = nullptr;
	Ammo = 0;
}

void UAmmoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UAmmoComponent, Ammo);
}

void UAmmoComponent::SetAmmo(int32 Count)
{
	Ammo = Count;
	OnAmmoUpdatedDelegate.Broadcast(Ammo);
}

void UAmmoComponent::ModifyAmmo(int32 Count)
{
	Ammo += Count;
	OnAmmoUpdatedDelegate.Broadcast(Ammo);
}

void UAmmoComponent::OnRep_Ammo()
{
	OnAmmoUpdatedDelegate.Broadcast(Ammo);
}

int32 UAmmoComponent::GetAmmo() const
{
	return Ammo;
}

bool UAmmoComponent::HasAmmo() const
{
	return Ammo > 0;
}