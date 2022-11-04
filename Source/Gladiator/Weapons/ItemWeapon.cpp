// Created by Bruce Crum


#include "Gladiator/Weapons/ItemWeapon.h"
#include "Gladiator/Items/InventoryComponent.h"
#include "Gladiator/Weapons/Ammo/AmmoComponent.h"

AItemWeapon::AItemWeapon()
{
	AmmoComponent = CreateDefaultSubobject<UAmmoComponent>(TEXT("AmmoComponent"));
}

void AItemWeapon::Equip()
{
	Super::Equip();

	// TODO: need to bind delegate for when we pick up ammo to update the ammo.
	UpdateAmmo();
}

void AItemWeapon::UpdateAmmo()
{
	if (GetOwner())
	{
		const auto Inventory = Cast<UInventoryComponent>(GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));
		if (Inventory)
		{
			const int32 Count = Inventory->GetAmmoAmountForType(AmmoComponent->AmmoType);
			AmmoComponent->SetAmmo(Count);
		}
	}
}