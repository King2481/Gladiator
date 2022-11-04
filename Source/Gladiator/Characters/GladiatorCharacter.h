// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GladiatorCharacter.generated.h"

class UInventoryComponent;
class UHealthComponent;
class AItemEquipable;
class UGladiatorAbilitySystemComponent;
class UGladiatorGameplayAbility;
class UCameraComponent;
class UItemSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipableChangedDelegate, AItemEquipable*, NewEquipable);

UCLASS()
class GLADIATOR_API AGladiatorCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGladiatorCharacter();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the actor ends play.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called to setup replication.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// The characters inventory.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComponent;

	// The characters health.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	UHealthComponent* HealthComponent;

	// The characters ability system.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UGladiatorAbilitySystemComponent* AbilitySystemComponent;

	// The Default abilities this character starts with.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGladiatorGameplayAbility>> StartingAbilities;

	// The Camera for this character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	/** First person character mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* ArmMesh1P;

	/** Weapon mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* WeaponMesh1P;

	// Called when the health component has reached 0
	UFUNCTION()
	void OnHealthDepleted(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// Attempts to kill the character
	void Die(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// Can this character actually die?
	bool CanDie(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) const;

	// Called when the character actually dies.
	void OnDeath();

	// Is the character dead?
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsDying, BlueprintReadOnly, Category = "Gladiator Character")
	bool bIsDying;

	UFUNCTION()
	void OnRep_IsDying();

	/* Reliably broadcasts a death event to clients, used to apply ragdoll forces */
	UFUNCTION(NetMulticast, Reliable)
	void BroadcastDeath(const FVector_NetQuantize& HitPosition, const FVector_NetQuantize& DamageForce, const FName& BoneName);
	void BroadcastDeath_Implementation(const FVector_NetQuantize& HitPosition, const FVector_NetQuantize& DamageForce, const FName& BoneName);

	// What currently equipped item is in this characters hand?
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentEquipable, BlueprintReadOnly, Category = "Gladiator Character")
	AItemEquipable* CurrentEquipable;

	UFUNCTION()
	void OnRep_CurrentEquipable();

	virtual void OnRep_PlayerState() override;

	// Binds the ability system component to the input system.
	void BindAbilitySystemToInputComponent();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipItem(AItemEquipable* Item);
	void ServerEquipItem_Implementation(AItemEquipable* Item);
	bool ServerEquipItem_Validate(AItemEquipable* Item);

	// Called when someone falls out of the world.
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	// What is the fall out of death damage type?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Gladiator Character")
	TSubclassOf<UDamageType> FellOutOfWorldDamageType;

	// Blueprint event for picking up the item.
	UFUNCTION(BlueprintImplementableEvent, Category = "Gladiator Character")
	void BlueprintOnDeath();

	void UpdateFirstPersonMeshes();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Attempts to equip the first available inventory item.
	void EquipFirstAvailableInventoryItem();

	// Equips an item
	UFUNCTION(BlueprintCallable, Category = "Gladiator Character")
	void EquipItem(AItemEquipable* Item);

	// Sets the current equipable
	void SetCurrentEquipable(AItemEquipable* Item, bool bFromReplication = false);

	UFUNCTION(BlueprintCallable, Category = "Gladiator Character")
	void PlayAnimationMontages(UAnimMontage* FirstPersonMontage, UAnimMontage* ThirdPersonMontage);

	// Called when our current equipable has changed.
	UPROPERTY(BlueprintAssignable)
	FOnEquipableChangedDelegate OnEquipableChangedDelegate;

	/*UFUNCTION(BlueprintCallable, Category = "Gladiator Character")
	void OnSelectInventoryPrevious();

	UFUNCTION(BlueprintCallable, Category = "Gladiator Character")
	void OnSelectInventoryNext();*/

	UFUNCTION(BlueprintCallable, Category = "Gladiator Character")
	AItemEquipable* GetPreviousItemInSlot(UItemSlot* Slot, bool bFallbackToLast);

	UFUNCTION(BlueprintCallable, Category = "Gladiator Character")
	AItemEquipable* GetNextItemInSlot(UItemSlot* Slot, bool bFallbackToFirst);
};
