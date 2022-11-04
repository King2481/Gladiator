// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "TeamInfo.generated.h"

class UTeamDefinition;
class AGladiatorPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayersArrayUpdatedDelegate);


/**
 * 
 */
UCLASS()
class GLADIATOR_API ATeamInfo : public AInfo
{
	GENERATED_BODY()

public:

	ATeamInfo();

	// Initializes this teaam with a definition and TeamId
	void InitializeTeam(UTeamDefinition* NewTeamDefition);

	// Variable replication setup.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Scores a specified amount of points (default is 1)
	UFUNCTION(BlueprintCallable, Category = "Team")
	void ScorePoints(int32 AmountToScore = 1);

	// Returns the score for this team
	int32 GetScore() const;

	// Adds a player to this team.
	void AddPlayer(AGladiatorPlayerState* NewPlayer);

	// Returns the team color
	FColor GetTeamColor() const;

protected:

	// What is the team definition? (ie: Red Team, Blue Team, etc.)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Team")
	UTeamDefinition* TeamDefinition;

	// What is this teams score?
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Team")
	int32 TeamScore;

	// What players are on this team?
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Players, BlueprintReadOnly, Category = "Team")
	TArray<AGladiatorPlayerState*> Players;

	UFUNCTION()
	void OnRep_Players();

	// Broadcasted out whenever players are added/removed
	UPROPERTY(BlueprintAssignable)
	FOnPlayersArrayUpdatedDelegate OnPlayersArrayUpdatedDelegate;
	
};
