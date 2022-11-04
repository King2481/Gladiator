// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "UIGameplayStatics.generated.h"

class APlayerState;

/**
 * 
 */
UCLASS()
class GLADIATOR_API UUIGameplayStatics : public UGameplayStatics
{
	GENERATED_BODY()

public:

	static FText GenerateKillFeedTextFromContexts(const UDamageType* InDamageType, APlayerState* KillerPS, APlayerState* VictimPS);

};
