// Created by Bruce Crum


#include "Gladiator/GameModes/GladiatorGameState.h"
#include "Gladiator/GameModes/GladiatorGameModeBase.h"
#include "Net/UnrealNetwork.h"

AGladiatorGameState::AGladiatorGameState()
{
	RoundEndTime = -1.0f;
	bRoundTimerExpired = false;
}

void AGladiatorGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGladiatorGameState, Teams);
	DOREPLIFETIME(AGladiatorGameState, RoundEndTime);
}

void AGladiatorGameState::AddTeam(ATeamInfo* NewTeam)
{
	Teams.AddUnique(NewTeam);
}

void AGladiatorGameState::SetRoundTimer(const int32 Seconds)
{
	if (HasAuthority())
	{
		RoundEndTime = GetServerWorldTimeSeconds() + Seconds;
		bRoundTimerExpired = false;

		// Reset timer
		if (GetWorld()->GetTimerManager().IsTimerActive(RoundTimeTimerHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(RoundTimeTimerHandle);
		}

		GetWorld()->GetTimerManager().SetTimer(RoundTimeTimerHandle, this, &AGladiatorGameState::TickTimer, 1.0f, true);
	}
}

void AGladiatorGameState::TickTimer()
{
	const float RoundTimeRemaining = GetRoundTimeRemaining();

	// not expired
	if (RoundTimeRemaining > 0 || bRoundTimerExpired)
	{
		return;
	}

	// let MP gamemode know if the timer has run out
	const auto GM = GetWorld()->GetAuthGameMode<AGladiatorGameModeBase>();
	if (GM && GM->OnRoundTimerExpired())
	{
		bRoundTimerExpired = true;
	}
}

float AGladiatorGameState::GetRoundTimeRemaining() const
{
	return FMath::Max(RoundEndTime - GetServerWorldTimeSeconds(), 0.0f);
}