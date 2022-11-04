// Created by Bruce Crum


#include "Gladiator/Abilities/GladiatorAbilitySystemComponent.h"
#include "Gladiator/Abilities/GladiatorGameplayAbility.h"

UGladiatorAbilitySystemComponent::UGladiatorAbilitySystemComponent()
{

}

void UGladiatorAbilitySystemComponent::GiveGameplayAbility(TSubclassOf<UGladiatorGameplayAbility> Ability)
{
	if (GetOwnerRole() != ROLE_Authority|| Ability == nullptr)
	{
		return;
	}

	GiveAbility(FGameplayAbilitySpec(Ability, 0, static_cast<int32>(Ability.GetDefaultObject()->InputID), this));
}

void UGladiatorAbilitySystemComponent::ClearGameplayAbility(TSubclassOf<UGladiatorGameplayAbility> Ability)
{
	if (GetOwnerRole() != ROLE_Authority || Ability == nullptr)
	{
		return;
	}

	// Check to see if there is a faster way to do this rather than iterating through the entire array.
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass() == Ability)
		{
			ClearAbility(FGameplayAbilitySpecHandle(Spec.Handle));
			break;
		}
	}
}