// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GladiatorGameState.generated.h"

class ATeamInfo;

/**
 * 
 */
UCLASS()
class GLADIATOR_API AGladiatorGameState : public AGameState
{
	GENERATED_BODY()

public:

	AGladiatorGameState();

	// Replication setup
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Adds a team to the game
	void AddTeam(ATeamInfo* NewTeam);

	void SetRoundTimer(const int32 Seconds);

	// Returns, in seconds, how much round time is remaining
	UFUNCTION(BlueprintPure, Category = "Game State")
	float GetRoundTimeRemaining() const;

protected:

	// What teams have been created and are currently in play?
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Gladiator Game State")
	TArray<ATeamInfo*> Teams;

	// At what time will the round in (in world seconds)
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Gladiator Game State")
	float RoundEndTime;

	// Has the round timer expired and no longer needs to tick?
	bool bRoundTimerExpired;

	// Ticks the round timer
	void TickTimer();

	FTimerHandle RoundTimeTimerHandle;
};
