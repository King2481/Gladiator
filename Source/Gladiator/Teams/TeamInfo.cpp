// Created by Bruce Crum


#include "Gladiator/Teams/TeamInfo.h"
#include "Gladiator/Teams/TeamDefinition.h"
#include "Net/UnrealNetwork.h"
#include "Gladiator/Player/GladiatorPlayerState.h"

ATeamInfo::ATeamInfo()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	TeamDefinition = nullptr;
	TeamScore = 0;
}

void ATeamInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamInfo, TeamDefinition);
	DOREPLIFETIME(ATeamInfo, TeamScore);
	DOREPLIFETIME(ATeamInfo, Players);
}

void ATeamInfo::InitializeTeam(UTeamDefinition* NewTeamDefition)
{
	TeamDefinition = NewTeamDefition;
}

void ATeamInfo::ScorePoints(int32 AmountToScore /* = 1 */)
{
	TeamScore += AmountToScore;
}

int32 ATeamInfo::GetScore() const
{
	return TeamScore;
}

void ATeamInfo::AddPlayer(AGladiatorPlayerState* NewPlayer)
{
	if (NewPlayer)
	{
		Players.AddUnique(NewPlayer);
		OnPlayersArrayUpdatedDelegate.Broadcast();
	}
}

void ATeamInfo::OnRep_Players()
{
	OnPlayersArrayUpdatedDelegate.Broadcast();
}

FColor ATeamInfo::GetTeamColor() const
{
	return TeamDefinition ? TeamDefinition->TeamColor : FColor::White;
}
