// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GladiatorPlayerState.generated.h"

class UKillTrackingComponent;

/**
 * 
 */
UCLASS()
class GLADIATOR_API AGladiatorPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AGladiatorPlayerState();

	// Scores a kill for this PlayerState
	void ScoreKill();

	// Scores a death for this PlayerState
	void ScoreDeath();

protected:

	// The kill tracking component this PlayerState uses
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gladiator Player State")
	UKillTrackingComponent* KillTrackingComponent;
	
};
