// Created by Bruce Crum


#include "Gladiator/Characters/GladiatorCharacter.h"
#include "Gladiator/Items/InventoryComponent.h"
#include "Gladiator/GameFramework/Components/HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Gladiator/GameModes/GladiatorGameModeBase.h"
#include "AbilitySystemComponent.h"
#include "Gladiator/Abilities/GladiatorGameplayAbility.h"
#include "Gladiator/Abilities/GladiatorAbilitySystemComponent.h"
#include "Gladiator/GameFramework/GladiatorDamageType.h"
#include "Gladiator/Items/ItemEquipable.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AGladiatorCharacter::AGladiatorCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	AbilitySystemComponent = CreateDefaultSubobject<UGladiatorAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = true;

	ArmMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh1P"));
	ArmMesh1P->SetupAttachment(CameraComponent);
	ArmMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	ArmMesh1P->CastShadow = false;
	ArmMesh1P->bOnlyOwnerSee = true;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	WeaponMesh1P->SetupAttachment(ArmMesh1P, "WeaponGripPoint");
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->bOnlyOwnerSee = true;

	bReplicates = true;

	bIsDying = false;
	CurrentEquipable = nullptr;
	FellOutOfWorldDamageType = nullptr;
}

// Called when the game starts or when spawned
void AGladiatorCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnHealthDepletedDelegate.AddDynamic(this, &AGladiatorCharacter::OnHealthDepleted);
	
	// Add the init here since we want clients to initialize the AbilitySystemComponent.
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	for (TSubclassOf<UGladiatorGameplayAbility> Ability : StartingAbilities)
	{
		AbilitySystemComponent->GiveGameplayAbility(Ability);
	}

	if (HasAuthority())
	{
		EquipFirstAvailableInventoryItem();
	}
}

void AGladiatorCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HealthComponent->OnHealthDepletedDelegate.RemoveDynamic(this, &AGladiatorCharacter::OnHealthDepleted);

	Super::EndPlay(EndPlayReason);
}

void AGladiatorCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AGladiatorCharacter, bIsDying);
	DOREPLIFETIME(AGladiatorCharacter, CurrentEquipable);
}

// Called every frame
void AGladiatorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGladiatorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	BindAbilitySystemToInputComponent();

}

void AGladiatorCharacter::OnHealthDepleted(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Die(Damage, DamageType, InstigatedBy, DamageCauser);
}

void AGladiatorCharacter::Die(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!CanDie(Damage, DamageType, InstigatedBy, DamageCauser))
	{
		return;
	}

	const auto GM = GetWorld()->GetAuthGameMode<AGladiatorGameModeBase>();
	if (GM)
	{
		// Inform the Game mode.
		GM->OnCharacterKilled(this, Damage, DamageType, InstigatedBy, DamageCauser);
	}

	FHitResult HitInfo;
	FVector MomentumDir;
	FDamageEvent DamageEvent;
	DamageEvent.GetBestHitInfo(this, DamageCauser, HitInfo, MomentumDir);

	const auto DamageTypeCasted = Cast<UGladiatorDamageType>(DamageType);
	const float LaunchMagnitude = DamageTypeCasted ? DamageTypeCasted->RagdollLaunchMagnitude : 12500.f;

	BroadcastDeath(HitInfo.ImpactPoint, MomentumDir * LaunchMagnitude, HitInfo.BoneName);

	OnDeath();
}

bool AGladiatorCharacter::CanDie(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) const
{
	if (bIsDying										// already dying
		|| !IsValid(this)							// already destroyed
		|| GetLocalRole() != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode<AGladiatorGameModeBase>() == NULL
		|| GetWorld()->GetAuthGameMode<AGladiatorGameModeBase>()->GetMatchState() == MatchState::LeavingMap) // level transition occurring
	{
		return false;
	}

	return true;
}

void AGladiatorCharacter::OnDeath()
{
	bIsDying = true;
	DetachFromControllerPendingDestroy();
	SetReplicateMovement(false);
	TearOff();
	SetLifeSpan(10.f);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BlueprintOnDeath();
}

void AGladiatorCharacter::BroadcastDeath_Implementation(const FVector_NetQuantize& HitPosition, const FVector_NetQuantize& DamageForce, const FName& BoneName)
{
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->AddImpulseAtLocation(DamageForce, HitPosition, BoneName);
}

void AGladiatorCharacter::OnRep_IsDying()
{
	OnDeath();
}

void AGladiatorCharacter::OnRep_CurrentEquipable()
{
	OnEquipableChangedDelegate.Broadcast(CurrentEquipable);
	UpdateFirstPersonMeshes();
}

void AGladiatorCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Note sure if we need this, but GASDocumentation does this to avoid a race condition, adding just in case.
	BindAbilitySystemToInputComponent();
}

void AGladiatorCharacter::BindAbilitySystemToInputComponent()
{
	if (IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("EAbilityInputID"), static_cast<int32>(EAbilityInputID::Confirm), static_cast<int32>(EAbilityInputID::Cancel)));
	}
}

void AGladiatorCharacter::EquipFirstAvailableInventoryItem()
{
	for (AItemBase* Item : InventoryComponent->GetItems())
	{
		if (!Item)
		{
			continue;
		}

		const auto Equipable = Cast<AItemEquipable>(Item);
		if (Equipable && Equipable->CanEquip())
		{
			EquipItem(Equipable);
			return;
		}
	}
}

void AGladiatorCharacter::EquipItem(AItemEquipable* Item)
{
	if (!HasAuthority())
	{
		ServerEquipItem(Item);
	}

	SetCurrentEquipable(Item);
}

void AGladiatorCharacter::ServerEquipItem_Implementation(AItemEquipable* Item)
{
	EquipItem(Item);
}

bool AGladiatorCharacter::ServerEquipItem_Validate(AItemEquipable* Item)
{
	return true;
}

void AGladiatorCharacter::SetCurrentEquipable(AItemEquipable* Item, bool bFromReplication /*= false*/)
{
	if (CurrentEquipable)
	{
		CurrentEquipable->Unequip();
	}

	CurrentEquipable = Item;

	if (CurrentEquipable)
	{
		CurrentEquipable->Equip();
		UpdateFirstPersonMeshes();
	}
	else
	{
		// Just hide it if we no longer have a valid mesh.
		WeaponMesh1P->SetHiddenInGame(true);
	}

	OnEquipableChangedDelegate.Broadcast(CurrentEquipable);
}

void AGladiatorCharacter::UpdateFirstPersonMeshes()
{
	if (CurrentEquipable)
	{
		WeaponMesh1P->SetHiddenInGame(false);
		WeaponMesh1P->SetSkeletalMesh(CurrentEquipable->GetMesh1PAsset());

		TArray<UMaterialInterface*> Materials = CurrentEquipable->GetMesh3P()->GetMaterials();
		for (int i = 0; i <= Materials.Num() - 1; i++)
		{
			if (Materials[i])
			{
				// Transfer all materials from the 3rd person mesh to the first person so it's 1:1
				WeaponMesh1P->SetMaterial(i, Materials[i]);
			}
		}
	}
}

/*void AGladiatorCharacter::OnSelectInventoryPrevious()
{
	if (!CurrentEquipable)
	{
		EquipFirstAvailableInventoryItem();
		return;
	}

	if (!AllowWeaponSwapping())
	{
		return;
	}

	AItemEquipable* ActiveItem = CurrentEquipable;
	AItemEquipable* Item = nullptr;

	// if we have a weapon, we want to try the current slot first, then the next one
	int CurrentSlot = (int)ActiveItem->ItemSlot;
	int OriginalSlot = CurrentSlot;
	Item = GetPreviousItemInSlot((EItemSlot)CurrentSlot, ActiveItem, false);

	// nothing else in the current slot, move on
	while (Item == nullptr)
	{
		--CurrentSlot;

		if (CurrentSlot < (int)EItemSlot::IS_Shotgun)
		{
			return;
		}

		// if we got back to our original slot, give up
		if (CurrentSlot == OriginalSlot)
		{
			return;
		}

		Item = GetPreviousItemInSlot((EItemSlot)CurrentSlot, nullptr, false);
	}

	// invalid item
	if (Item == nullptr)
	{
		return;
	}

	EquipItem(Item);
}

void AGladiatorCharacter::OnSelectInventoryNext()
{
	if (!CurrentEquipable)
	{
		EquipFirstAvailableInventoryItem();
		return;
	}

	if (!AllowWeaponSwapping())
	{
		return;
	}

	AItemEquipable* ActiveItem = CurrentEquipable;
	AItemEquipable* Item = nullptr;

	// if we have a weapon, we want to try the current slot first, then the next one
	int CurrentSlot = (int)ActiveItem->ItemSlot;
	int OriginalSlot = CurrentSlot;
	Item = GetNextItemInSlot((EItemSlot)CurrentSlot, ActiveItem, false);

	// nothing else in the current slot, move on
	while (Item == nullptr)
	{
		++CurrentSlot;

		// roll over back to primary
		if (CurrentSlot >= (int)EItemSlot::IS_Count)
		{
			return;
		}

		// if we got back to our original slot, give up
		if (CurrentSlot == OriginalSlot)
		{
			return;
		}

		Item = GetNextItemInSlot((EItemSlot)CurrentSlot, nullptr, false);
	}

	// invalid item
	if (Item == nullptr)
	{
		return;
	}

	EquipItem(Item);
}*/

AItemEquipable* AGladiatorCharacter::GetNextItemInSlot(UItemSlot* Slot, bool bFallbackToFirst)
{
	const bool bCurrentInSameSlot = (CurrentEquipable && CurrentEquipable->ItemSlot == Slot); // Are we transitioning from a weapon in the same slot?
	bool bHasPassedCurrentItem = false;
	AItemEquipable* FirstItemInSlot = nullptr;

	// Iterate through our entire inventory
	for (AItemBase* Item : InventoryComponent->GetItems())
	{
		const auto Equipable = Cast<AItemEquipable>(Item);

		// Skip null items or anything not matching our slot
		if (!Equipable || Equipable->IsPendingKillPending() || Equipable->ItemSlot != Slot || !Equipable->CanEquip())
		{
			continue;
		}

		// Not transitioning from an item in the same slot, the first item is fine
		if (!bCurrentInSameSlot)
		{
			return Equipable;
		}

		// Record the first item we encounter in this specific slot
		if (FirstItemInSlot == nullptr)
		{
			FirstItemInSlot = Equipable;
		}

		if (!bHasPassedCurrentItem)
		{
			// Flag if we've reached our CurrentItem, the next weapon in this slot is good to use
			bHasPassedCurrentItem = (CurrentEquipable == Equipable);
		}
		else
		{
			// We've passed our CurrentItem and hit a new weapon in the same slot
			return Equipable;
		}
	}

	// Unable to find a "next" item, assume that we reached the end of our inventory and are looping back to the first
	if (bFallbackToFirst && bHasPassedCurrentItem && FirstItemInSlot != CurrentEquipable)
	{
		return FirstItemInSlot;
	}

	return nullptr;
}

AItemEquipable* AGladiatorCharacter::GetPreviousItemInSlot(UItemSlot* Slot, bool bFallbackToLast)
{
	const bool bCurrentInSameSlot = (CurrentEquipable && CurrentEquipable->ItemSlot == Slot); // Are we transitioning from a weapon in the same slot?
	bool bHasPassedCurrentItem = false;
	AItemEquipable* LastItemInSlot = nullptr;

	TArray<AItemBase*> Items = InventoryComponent->GetItems();

	// Iterate through our entire inventory backwards
	for (int i = Items.Num() - 1; i >= 0; --i)
	{
		const auto Item = Cast<AItemEquipable>(Items[i]);

		// Skip null items or anything not matching our slot
		if (!Item || Item->ItemSlot != Slot || !Item->CanEquip())
		{
			continue;
		}

		// Not transitioning from an item in the same slot, the first item is fine
		if (!bCurrentInSameSlot)
		{
			return Item;
		}

		// Record the first item we encounter in this specific slot (which will be the last one in the slot)
		if (LastItemInSlot == nullptr)
		{
			LastItemInSlot = Item;
		}

		if (!bHasPassedCurrentItem)
		{
			// Flag if we've reached our CurrentItem, the next weapon in this slot is good to use
			bHasPassedCurrentItem = (CurrentEquipable == Item);
		}
		else
		{
			// We've passed our CurrentItem and hit a new weapon in the same slot
			return Item;
		}
	}

	// Unable to find a "previous" item, assume that we reached the end of our inventory and are looping back to the last
	if (bFallbackToLast && bHasPassedCurrentItem && LastItemInSlot != CurrentEquipable)
	{
		return LastItemInSlot;
	}

	return nullptr;
}


void AGladiatorCharacter::PlayAnimationMontages(UAnimMontage* FirstPersonMontage, UAnimMontage* ThirdPersonMontage)
{
	if (FirstPersonMontage && ArmMesh1P->GetAnimInstance())
	{
		ArmMesh1P->GetAnimInstance()->Montage_Play(FirstPersonMontage);
	}

	if (ThirdPersonMontage && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(ThirdPersonMontage);
	}
}

void AGladiatorCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	Die(HealthComponent->GetHealth(), &dmgType, GetController(), nullptr);
}