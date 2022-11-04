// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GladiatorGameModeBase.generated.h"

class AGladiatorCharacter;
class UTeamDefinition;
class UInGameState;
class AGladiatorCharacter;
class AGladiatorPlayerState;

/**
* Additional match states
*/
namespace MatchState
{
	/* round has ended and a winner (or draw) has been declared */
	extern const FName RoundWon;
}

/**
 * 
 */
UCLASS()
class GLADIATOR_API AGladiatorGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	
	AGladiatorGameModeBase();

	// Called when the game is initialized
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	// Called when components have been initialized.
	virtual void PostInitializeComponents() override;

	// Called when a character is killed.
	virtual void OnCharacterKilled(AGladiatorCharacter* Victim, float KillingDamage, const UDamageType* DamageType, AController* EventInstigator, AActor* DamageCauser);

	// Changes the InGameState, MatchState is then called based on the data. 
	UFUNCTION(BlueprintCallable, Category = "Gladiator GameMode")
	void SetInGameState(UInGameState* NewInGameState);

	// Called when the Gamemode has reached a the winning condition and there is a solo player that has won.
	UFUNCTION(BlueprintCallable, Category = "Gladiator GameMode")
	void PlayerStateWin(AGladiatorPlayerState* NewWinningPlayerState);

	// Called when the Gamemode has reached a the winning condition and there is team that has won.
	UFUNCTION(BlueprintCallable, Category = "Gladiator GameMode")
	void TeamWin(const uint8 NewWinningTeam);

	// Notifies all PlayerControllers that the match is over and a round was won.
	UFUNCTION(BlueprintCallable, Category = "Gladiator GameMode")
	void NotifyPlayersOfRoundWon();

	// Sets the round timer 
	UFUNCTION(BlueprintCallable, Category = "Gladiator GameMode")
	void SetRoundTimer(const int32 Timer);

	// Called when the GameState's timer has expired, GameMode can decide whether it ends or not.
	virtual bool OnRoundTimerExpired();

protected:

	// What teams are in this mode?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gladiator GameMode")
	TArray<TSoftObjectPtr<UTeamDefinition>> TeamsForMode;

	// Initializes the team for this game mode if there are any.
	virtual void InitializeTeams();

	// A generic score needed to "win" this GameMode
	UPROPERTY(BlueprintReadOnly, Category = "Gladiator GameMode")
	int32 ScoreNeededToWin;

	// Is the killfeed enabled?
	UPROPERTY(BlueprintReadOnly, Category = "Gladiator GameMode")
	bool bKillFeed;

	// What is the current Data Driven InGameState?
	UPROPERTY(BlueprintReadOnly, Category = "Gladiator GameMode")
	UInGameState* CurrentInGameState;

	// This sole winner of this gamemode.
	UPROPERTY(BlueprintReadWrite, Category = "Gladiator GameMode")
	AGladiatorPlayerState* WinningPlayerState;

	// The winning team of this gamemode
	UPROPERTY(BlueprintReadWrite, Category = "Gladiator GameMode")
	uint8 WinningTeamId;

	// How long does this game mode last, in seconds?
	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category = "Gamemode")
	int32 RoundTimeLimit;

	// Blueprint event for a character death
	UFUNCTION(BlueprintImplementableEvent, Category = "Gamemode")
	void BlueprintOnCharacterKilled(AGladiatorCharacter* Victim, float KillingDamage, const UDamageType* DamageType, AController* EventInstigator, AActor* DamageCauser);

	// Calls when the match has started
	virtual void HandleMatchHasStarted() override;
};
