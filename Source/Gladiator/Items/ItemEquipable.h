// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Gladiator/Items/ItemBase.h"
#include "ItemEquipable.generated.h"

class UGladiatorGameplayAbility;
class UGladiatorAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEquipableState : uint8
{
	Unequipped 	  UMETA(DisplayName = "Unequipped"),
	Equipping	  UMETA(DisplayName = "Equipping"),
	Equipped      UMETA(DisplayName = "Equipped")
};

USTRUCT(BlueprintType)
struct FAnimationPair
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UAnimMontage* FirstPersonAnim;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UAnimMontage* ThirdPersonAnim;

	FAnimationPair()
	{
		FirstPersonAnim = nullptr;
		ThirdPersonAnim = nullptr;
	}
};
 
/**
 * 
 */
UCLASS()
class GLADIATOR_API AItemEquipable : public AItemBase
{
	GENERATED_BODY()

public:

	AItemEquipable();

	// Called when we equip, passed AbilitySystemComponent to give abilities to on equip.
	virtual void Equip();

	// Called when we unequip, passed AbilitySystemComponent to remove abilities to on unequip.
	virtual void Unequip();

	// Can we equip this item?
	virtual bool CanEquip() const;

	// Initializes the item
	virtual void InitItem(AActor* NewOwner) override;

	// What gameplay abilities are granted on equip?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item Equipable")
	TArray<TSubclassOf<UGladiatorGameplayAbility>> GameplayAbilitiesToGiveOnEquip;

	// Set the equipable state.
	UFUNCTION(BlueprintCallable, Category = "Item Equipable")
	void SetEquipableState(EEquipableState NewState);

	// Returns the current equipable state of the weapon, will return simulated equippable state on remote clients.
	UFUNCTION(BlueprintPure, Category = "Item Equipable")
	EEquipableState GetEquipableState() const;

	// Checks to see if we are currently swapping to this weapon.
	UFUNCTION(BlueprintPure, Category = "Item Equipable")
	bool IsSwappingTo() const;

	USkeletalMesh* GetMesh1PAsset() const;

	USkeletalMeshComponent* GetMesh3P() const;

protected:

	// Item mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* Mesh3P;

	// The mesh that the first person mesh will use.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMesh* Mesh1PAsset;

	// Variable replication setup.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// The current equipable state of the item.
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_EquipableState, BlueprintReadOnly, Category = "Item Equipable")
	EEquipableState EquipableState;

	// The simulated equipable state of the item (clients)
	UPROPERTY(BlueprintReadOnly, Category = "Item Equipable")
	EEquipableState SimulatedEquipableState;

	UFUNCTION()
	void OnRep_EquipableState();

	// Called when the equipable state has changed.
	void OnEquipableStateChanged();

	// Called when equipped.
	virtual void OnEquipped();

	// Called when Unequipped
	virtual void OnUnequipped();

	// Called when we have finished swapping to this item.
	void OnSwapToFinished();

	// How much time does it take to swap to this weapon?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item Equipable")
	float SwapToTime;

	UPROPERTY()
	FTimerHandle SwapToTimerHandle;

	// On the character mesh, where does this weapon attach to?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item Equipable")
	FName AttachEquipableSocketName;

	// The linked ability system component this item uses.
	UGladiatorAbilitySystemComponent* LinkedAbilitySystemComponent;

	// The equip to (ie: switching to) animation pair for this item.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
	FAnimationPair EquipAnimationPair;
};
