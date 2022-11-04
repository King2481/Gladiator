// Created by Bruce Crum


#include "Gladiator/Player/GladiatorPlayerState.h"
#include "Gladiator/Player/Components/KillTrackingComponent.h"

AGladiatorPlayerState::AGladiatorPlayerState()
{
	KillTrackingComponent = CreateDefaultSubobject<UKillTrackingComponent>(TEXT("Kill Tracking Component"));
}

void AGladiatorPlayerState::ScoreKill()
{
	KillTrackingComponent->OnKillScored();
}

void AGladiatorPlayerState::ScoreDeath()
{
	KillTrackingComponent->OnDeathScored();
}