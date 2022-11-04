// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Gladiator/Items/ItemEquipable.h"
#include "ItemWeapon.generated.h"

class UAmmoComponent;

/**
 * 
 */
UCLASS()
class GLADIATOR_API AItemWeapon : public AItemEquipable
{
	GENERATED_BODY()

public:

	AItemWeapon();

	virtual void Equip() override;
	
	// The Stored ammo for this weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Weapon")
	UAmmoComponent* AmmoComponent;

protected:

	void UpdateAmmo();
};
