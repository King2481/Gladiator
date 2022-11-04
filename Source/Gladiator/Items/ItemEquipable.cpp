// Created by Bruce Crum


#include "Gladiator/Items/ItemEquipable.h"
#include "Gladiator/Abilities/GladiatorAbilitySystemComponent.h"
#include "Gladiator/Abilities/GladiatorGameplayAbility.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

AItemEquipable::AItemEquipable()
{
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P"));
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->CastShadow = true;
	Mesh3P->bOnlyOwnerSee = false;
	Mesh3P->bOwnerNoSee = true;

	SetRootComponent(Mesh3P);

	Mesh1PAsset = nullptr;
	AttachEquipableSocketName = FName("EquipableAttachPoint");

	EquipableState = EEquipableState::Unequipped;
	SimulatedEquipableState = EEquipableState::Unequipped;

	SwapToTime = 0.5f;
}

void AItemEquipable::InitItem(AActor* NewOwner)
{
	Super::InitItem(NewOwner);

	SetActorHiddenInGame(true);

	const auto Character = Cast<ACharacter>(NewOwner);
	if (Character) // If the character is holding a weapon, attach it to their mesh.
	{
		AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachEquipableSocketName);
	}

	const auto ASC = GetOwner()->FindComponentByClass<UGladiatorAbilitySystemComponent>();
	if (ASC)
	{
		LinkedAbilitySystemComponent = ASC;
	}
	else
	{
		UE_LOG(LogItem, Warning, TEXT("Warning: AItemEquipable::InitItem(), the owner of %s does not have a GladiatorAbilitySystemComponent, is this intentional?"), *GetNameSafe(this));
	}
}

void AItemEquipable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemEquipable, EquipableState);
}

void AItemEquipable::Equip()
{
	if (LinkedAbilitySystemComponent)
	{
		for (TSubclassOf<UGladiatorGameplayAbility> Ability : GameplayAbilitiesToGiveOnEquip)
		{
			LinkedAbilitySystemComponent->GiveGameplayAbility(Ability);
		}
	}

	SetActorHiddenInGame(false);

	GetWorldTimerManager().SetTimer(SwapToTimerHandle, this, &AItemEquipable::OnSwapToFinished, SwapToTime);
}

void AItemEquipable::OnSwapToFinished()
{
	SetEquipableState(EEquipableState::Equipped);
}

void AItemEquipable::Unequip()
{
	SetEquipableState(EEquipableState::Unequipped);
}

void AItemEquipable::OnEquipped()
{

}

void AItemEquipable::OnUnequipped()
{
	if (LinkedAbilitySystemComponent)
	{
		for (TSubclassOf<UGladiatorGameplayAbility> Ability : GameplayAbilitiesToGiveOnEquip)
		{
			LinkedAbilitySystemComponent->ClearGameplayAbility(Ability);
		}
	}

	SetActorHiddenInGame(true);

	GetWorldTimerManager().ClearTimer(SwapToTimerHandle);
}

void AItemEquipable::SetEquipableState(EEquipableState NewState)
{
	EquipableState = NewState;
	SimulatedEquipableState = NewState;

	OnEquipableStateChanged();
}

EEquipableState AItemEquipable::GetEquipableState() const
{
	if (!HasAuthority())
	{
		return SimulatedEquipableState;
	}

	return EquipableState;
}

void AItemEquipable::OnRep_EquipableState()
{
	if (EquipableState == SimulatedEquipableState)
	{
		return;
	}

	OnEquipableStateChanged();
}

void AItemEquipable::OnEquipableStateChanged()
{
	switch (EquipableState)
	{
		case EEquipableState::Equipping:
		{
			Equip();
			break;
		}

		case EEquipableState::Equipped:
		{
			OnEquipped();
			break;
		}

		case EEquipableState::Unequipped:
		{
			OnUnequipped();
			break;
		}

		default:
		{
			// Do nothing
			break;
		}
	}
}

bool AItemEquipable::CanEquip() const
{
	return true;
}

bool AItemEquipable::IsSwappingTo() const
{
	return GetEquipableState() == EEquipableState::Equipping;
}

USkeletalMesh* AItemEquipable::GetMesh1PAsset() const
{
	return Mesh1PAsset;
}

USkeletalMeshComponent* AItemEquipable::GetMesh3P() const
{
	return Mesh3P;
}