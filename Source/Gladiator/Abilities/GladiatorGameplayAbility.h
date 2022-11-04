// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GladiatorGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None			UMETA(DisplayName = "None"),
	Confirm			UMETA(DisplayName = "Confirm"),
	Cancel			UMETA(DisplayName = "Cancel"),
	Fire			UMETA(DisplayName = "Fire"), // Left Mouse
	AltFire			UMETA(DisplayName = "AltFire"), // Right Mouse
	Jump            UMETA(DisplayName = "Jump"), // Space Bar
	SwitchToSlot1   UMETA(DisplayName = "Slot1"), // 1
	SwitchToSlot2   UMETA(DisplayName = "Slot2"), // 2
	SwitchToSlot3   UMETA(DisplayName = "Slot3"), // 3
	SwitchToSlot4   UMETA(DisplayName = "Slot4"), // 4
	SwitchToSlot5   UMETA(DisplayName = "Slot5"), // 5
};

/**
 * 
 */
UCLASS()
class GLADIATOR_API UGladiatorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGladiatorGameplayAbility();

	// What is the input ID associated with this ability?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	EAbilityInputID InputID;
};
