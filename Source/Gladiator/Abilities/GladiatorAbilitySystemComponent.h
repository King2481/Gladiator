// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GladiatorAbilitySystemComponent.generated.h"

class UGladiatorGameplayAbility;

/**
 * 
 */
UCLASS()
class GLADIATOR_API UGladiatorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UGladiatorAbilitySystemComponent();

	// Grants a gameplay ability
	UFUNCTION(BlueprintCallable, Category = "Gladiator Character")
	void GiveGameplayAbility(TSubclassOf<UGladiatorGameplayAbility> Ability);

	// Removes a gameplay ability
	UFUNCTION(BlueprintCallable, Category = "Gladiator Character")
	void ClearGameplayAbility(TSubclassOf<UGladiatorGameplayAbility> Ability);
	
};
