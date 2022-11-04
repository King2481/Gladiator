// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KillFeedDisplayTextCollection.generated.h"

class UKillFeedDisplayText;

/**
 * 
 */
UCLASS()
class GLADIATOR_API UKillFeedDisplayTextCollection : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UKillFeedDisplayTextCollection();

	UPROPERTY(EditDefaultsOnly, Category = "Kill Feed Display Text Collection")
	TMap<TSubclassOf<UDamageType>, UKillFeedDisplayText*> DamageTypeToTextMap;
};
