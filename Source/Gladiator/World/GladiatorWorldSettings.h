// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "GladiatorWorldSettings.generated.h"

class USoundCue;

/**
 * 
 */
UCLASS()
class GLADIATOR_API AGladiatorWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	AGladiatorWorldSettings();

protected:

	/* What background music is playing for this level? */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gladiator World Settings")
	USoundCue* BackgroundMusicSoundCue;
	
};
