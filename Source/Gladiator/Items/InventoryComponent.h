// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AItemBase;
class UAmmoType;

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, Verbose);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoredAmmoUpdatedDelegate);

USTRUCT(BlueprintType)
struct FStoredAmmo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UAmmoType* AmmoType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 Ammo;

	FStoredAmmo()
	{
		AmmoType = nullptr;
		Ammo = 0;
	}

	FStoredAmmo(UAmmoType* Type, int32 StoreAmount)
	{
		AmmoType = Type;
		Ammo = StoreAmount;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLADIATOR_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UInventoryComponent();

	// Checks to see if we have the specified class in the inventory.
	UFUNCTION(BlueprintPure, Category = "Inventory Component")
	bool HasItemClassInInventory(TSubclassOf<AItemBase> ItemToFind);

	// Gives an item class and spawns it into the world.
	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
	void GiveItemClass(TSubclassOf<AItemBase> ItemToGive);

	// Returns the items currently stored in this inventory
	UFUNCTION(BlueprintPure, Category = "Inventory Component")
	TArray<AItemBase*> GetItems() const;

	// Returns the ammo currently stored in this inventory.
	UFUNCTION(BlueprintPure, Category = "Inventory Component")
	TArray<FStoredAmmo> GetAmmo() const;

	// Returns the amount of ammo we have for this type.
	UFUNCTION(BlueprintPure, Category = "Inventory Component")
	int32 GetAmmoAmountForType(UAmmoType* AmmoType) const;

	// Stores the current ammo. (Setter).
	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
	void StoreAmmo(UAmmoType* AmmoType, int32 AmountToStore);

	// Gives an ammount of ammo already on top of the current ammo. (Adder).
	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
	void GiveAmmo(UAmmoType* AmmoType, int32 AmountToGive);

	// Bindable delegate for when the ammo actually updates.
	UPROPERTY(BlueprintAssignable)
	FOnStoredAmmoUpdatedDelegate OnStoredAmmoUpdatedDelegate;

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the components ends play.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called to setup replication.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// What default items are spawned?
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Inventory Component")
	TArray<TSubclassOf<AItemBase>> DefaultInventoryItems;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Inventory Component")
	TArray<AItemBase*> Items;

	// The default ammo this inventory starts with.
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Inventory Component")
	TArray<FStoredAmmo> DefaultAmmo;

	// The stored ammo for this inventory
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_StoredAmmo, BlueprintReadOnly, Category = "Inventory Component")
	TArray<FStoredAmmo> StoredAmmo;

	UFUNCTION()
	void OnRep_StoredAmmo();

	// Gives any default inventory items we may have.
	void GiveDefaultInventoryItems();

	// Destroys all the inventory items this component may have.
	void DestroyAllInventoryItems();
};
