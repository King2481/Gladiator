// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InGameState.generated.h"

/**
 * 
 */
UCLASS()
class GLADIATOR_API UInGameState : public UDataAsset
{
	GENERATED_BODY()

public:

	UInGameState();

	// What is the Name of this state? Used by the GameMode to run custom game state logic
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "InGameState")
	FName StateName;
};
