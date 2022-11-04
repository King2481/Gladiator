// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KillFeedDisplayText.generated.h"

/**
 * 
 */
UCLASS()
class GLADIATOR_API UKillFeedDisplayText : public UDataAsset
{
	GENERATED_BODY()

public:

	UKillFeedDisplayText();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Kill Feed Display Text")
	TArray<FText> PossibleOptions;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Kill Feed Display Text")
	TArray<FText> PossibleSelfKillOptions;
	
};
