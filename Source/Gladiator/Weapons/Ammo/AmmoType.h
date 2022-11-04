// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AmmoType.generated.h"

/**
 * 
 */
UCLASS()
class GLADIATOR_API UAmmoType : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UAmmoType();

	// What is the maximum amount of ammo we are allowed to have for this type.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (ClampMin = "0"))
	int32 MaxAmount;
};
