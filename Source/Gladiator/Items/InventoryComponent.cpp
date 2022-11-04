// Created by Bruce Crum


#include "Gladiator/Items/InventoryComponent.h"
#include "Gladiator/Items/ItemBase.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogInventory);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() >= ROLE_Authority)
	{
		GiveDefaultInventoryItems();
		StoredAmmo = DefaultAmmo;
	}
}

void UInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyAllInventoryItems();

	Super::EndPlay(EndPlayReason);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UInventoryComponent, Items);

	// Owner only
	DOREPLIFETIME_CONDITION(UInventoryComponent, StoredAmmo, COND_OwnerOnly);
}

void UInventoryComponent::GiveDefaultInventoryItems()
{
	if (DefaultInventoryItems.Num() == 0)
	{
		UE_LOG(LogInventory, Warning, TEXT("%s owned by %s has no default items to give, is this intentional?"), *GetNameSafe(this), *GetNameSafe(GetOwner()));
		return;
	}

	for (auto& ItemClass : DefaultInventoryItems)
	{
		GiveItemClass(ItemClass);
	}
}

bool UInventoryComponent::HasItemClassInInventory(TSubclassOf<AItemBase> ItemToFind)
{
	for (auto& Item : Items)
	{
		if (Item && Item->GetClass() == ItemToFind)
		{
			return true;
		}
	}

	return false;
}

void UInventoryComponent::GiveItemClass(TSubclassOf<AItemBase> ItemToGive)
{
	if (GetOwnerRole() >= ROLE_Authority)
	{
		if (ItemToGive)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();

			const auto Item = GetWorld()->SpawnActor<AItemBase>(ItemToGive, SpawnParams);
			if (Item)
			{
				Item->InitItem(GetOwner());
				Items.AddUnique(Item);
			}
		}
		else
		{
			UE_LOG(LogInventory, Warning, TEXT("%s owned by %s attempted to give an item that does not have a valid class."), *GetNameSafe(this), *GetNameSafe(GetOwner()));
		}
	}
	else
	{
		UE_LOG(LogInventory, Warning, TEXT("%s owned by %s attempted to give an item but does not have ENetRole::ROLE_Authority."), *GetNameSafe(this), *GetNameSafe(GetOwner()));
	}
}

void UInventoryComponent::DestroyAllInventoryItems()
{
	if (GetOwnerRole() >= ROLE_Authority)
	{
		for (int i = Items.Num() - 1; i >= 0; i--)
		{
			AItemBase* Item = Items[i];
			if (Item)
			{
				Items.RemoveSingle(Item);
				Item->Destroy();
			}
		}

		Items.Empty();
	}
}

TArray<AItemBase*> UInventoryComponent::GetItems() const
{
	return Items;
}

TArray<FStoredAmmo> UInventoryComponent::GetAmmo() const
{
	return StoredAmmo;
}

void UInventoryComponent::GiveAmmo(UAmmoType* AmmoType, int32 AmountToGive)
{
	bool bFound = false;

	for (FStoredAmmo& AmmoSlot : StoredAmmo)
	{
		if (AmmoSlot.AmmoType != AmmoType)
		{
			continue;
		}

		AmmoSlot.Ammo += AmountToGive;
		bFound = true;
		break;
	}

	if (!bFound)
	{
		// Couldn't find it, add it to our stored ammo.
		FStoredAmmo NewAmmo(AmmoType, AmountToGive);
		StoredAmmo.Add(NewAmmo);
	}

	OnStoredAmmoUpdatedDelegate.Broadcast();
}

void UInventoryComponent::StoreAmmo(UAmmoType* AmmoType, int32 AmountToStore)
{
	for (FStoredAmmo& AmmoSlot : StoredAmmo)
	{
		if (AmmoSlot.AmmoType != AmmoType)
		{
			continue;
		}

		AmmoSlot.Ammo = AmountToStore;
		return;
	}

	// Couldn't find it, add it to our stored ammo.
	FStoredAmmo NewAmmo(AmmoType, AmountToStore);
	StoredAmmo.Add(NewAmmo);

	OnStoredAmmoUpdatedDelegate.Broadcast();
}

int32 UInventoryComponent::GetAmmoAmountForType(UAmmoType* AmmoType) const
{
	int32 Amount = 0;

	for (FStoredAmmo AmmoSlot : StoredAmmo)
	{
		if (AmmoSlot.AmmoType != AmmoType)
		{
			continue;
		}

		Amount = AmmoSlot.Ammo;
		break;
	}

	return Amount;
}

void UInventoryComponent::OnRep_StoredAmmo()
{
	OnStoredAmmoUpdatedDelegate.Broadcast();
}
