// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "GladiatorDamageType.generated.h"

/**
 * 
 */
UCLASS()
class GLADIATOR_API UGladiatorDamageType : public UDamageType
{
	GENERATED_BODY()

public:

	UGladiatorDamageType();

	// When the ragdoll launches, what is the launch magnitude (ie: how strong is the knockback?)
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Gladiator Damage Type")
	float RagdollLaunchMagnitude;

	// When the an actor launches, what is the launch magnitude (ie: how strong is the knockback?)
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Gladiator Damage Type")
	float LaunchMagnitude;

};
