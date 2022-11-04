// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GladiatorUISettings.generated.h"

class UKillFeedDisplayTextCollection;

/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Gladiator UI Settings"))
class GLADIATOR_API UGladiatorUISettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	UGladiatorUISettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	TSoftObjectPtr<UKillFeedDisplayTextCollection> KillFeedDisplayCollection;

};
