// Copyright Epic Games, Inc. All Rights Reserved.


#include "Gladiator/GameModes/GladiatorGameModeBase.h"
#include "Gladiator/GameModes/GladiatorGameState.h"
#include "Gladiator/Teams/TeamInfo.h"
#include "Gladiator/Teams/TeamDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "Gladiator/Player/GladiatorPlayerState.h"
#include "Gladiator/Characters/GladiatorCharacter.h"
#include "Gladiator/GameModes/InGameState.h"
#include "Gladiator/Player/GladiatorPlayerController.h"
#include "Gladiator/UI/UIGameplayStatics.h"

AGladiatorGameModeBase::AGladiatorGameModeBase()
{
	ScoreNeededToWin = 25;
	CurrentInGameState = nullptr;
	WinningPlayerState = nullptr;
	WinningTeamId = 255;
	RoundTimeLimit = 5; // 5 minutes by default
	bKillFeed = true;
}

void AGladiatorGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	RoundTimeLimit = UGameplayStatics::GetIntOption(Options, TEXT("RoundTimeLimit"), RoundTimeLimit) * 60; // Multiply the get it in seconds as the game counts seconds.
	ScoreNeededToWin = UGameplayStatics::GetIntOption(Options, TEXT("ScoreNeededToWin"), ScoreNeededToWin);
	bKillFeed = UGameplayStatics::GetIntOption(Options, TEXT("KillFeed"), bKillFeed) > 0;
}

void AGladiatorGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitializeTeams();
}

void AGladiatorGameModeBase::InitializeTeams()
{
	if (TeamsForMode.Num() <= 0)
	{
		// No teams to initialize
		return;
	}

	const auto GS = GetGameState<AGladiatorGameState>();
	if (GS)
	{
		for (int i = 0; i <= TeamsForMode.Num() - 1; i++)
		{
			UTeamDefinition* Team = TeamsForMode[i].LoadSynchronous();
			if (Team)
			{
				const auto NewTeam = GetWorld()->SpawnActor<ATeamInfo>(ATeamInfo::StaticClass(), FTransform());
				if (NewTeam)
				{
					NewTeam->InitializeTeam(Team);
					GS->AddTeam(NewTeam);
				}
			}
		}
	}
}

void AGladiatorGameModeBase::OnCharacterKilled(AGladiatorCharacter* Victim, float KillingDamage, const UDamageType* DamageType, AController* EventInstigator, AActor* DamageCauser)
{
	const auto VictimPS = Cast<AGladiatorPlayerState>(Victim->GetPlayerState());
	const auto KillerPS = EventInstigator ? EventInstigator->GetPlayerState<AGladiatorPlayerState>() : nullptr;
	const auto GS = Cast<AGladiatorGameState>(GameState);
	const bool bSelfKill = VictimPS == KillerPS;

	if (VictimPS && KillerPS)
	{
		UE_LOG(LogGameMode, VeryVerbose, TEXT("%s Was killed by %s"), *VictimPS->GetPlayerName(), *KillerPS->GetPlayerName());
	}

	if (KillerPS && !bSelfKill)
	{
		KillerPS->ScoreKill();
	}

	if (VictimPS)
	{
		VictimPS->ScoreDeath();
	}

	if (bKillFeed)
	{
		const FText Notice = UUIGameplayStatics::GenerateKillFeedTextFromContexts(DamageType, KillerPS, VictimPS);

		for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
		{
			const auto Controller = Cast<AGladiatorPlayerController>(Iterator->Get());
			if (Controller)
			{
				Controller->ClientRecieveAnnouncementNotice(Notice);
			}
		}
	}

	BlueprintOnCharacterKilled(Victim, KillingDamage, DamageType, EventInstigator, DamageCauser);
}

void AGladiatorGameModeBase::SetInGameState(UInGameState* NewInGameState)
{
	CurrentInGameState = NewInGameState;

	SetMatchState(CurrentInGameState->StateName);
}

void AGladiatorGameModeBase::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	SetRoundTimer(RoundTimeLimit);
}

void AGladiatorGameModeBase::PlayerStateWin(AGladiatorPlayerState* NewWinningPlayerState)
{
	WinningPlayerState = NewWinningPlayerState;	
}

void AGladiatorGameModeBase::TeamWin(const uint8 NewWinningTeam)
{
	WinningTeamId = NewWinningTeam;
}

void AGladiatorGameModeBase::NotifyPlayersOfRoundWon()
{
	// Inform all connected players that we have won the round.
	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		const auto Controller = Cast<AGladiatorPlayerController>(Iterator->Get());
		if (Controller)
		{
			Controller->OnRoundWon(WinningPlayerState, WinningTeamId);
		}
	}
}

void AGladiatorGameModeBase::SetRoundTimer(const int32 Timer)
{
	const auto GS = GetWorld()->GetGameState<AGladiatorGameState>();
	if (GS)
	{
		GS->SetRoundTimer(Timer);
	}
}

bool AGladiatorGameModeBase::OnRoundTimerExpired()
{
	return true;
}