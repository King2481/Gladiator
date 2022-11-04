// Created by Bruce Crum


#include "Gladiator/Player/Components/KillTrackingComponent.h"
#include "Gladiator/Player/GladiatorPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UKillTrackingComponent::UKillTrackingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	CurrentSpree = 0;
	CurrentStreakCombo = 0;
	KillingStreakFinishTime = 2.5f;
	ComboEndTime = -1.0f;
}

void UKillTrackingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UKillTrackingComponent, MatchStats);

	// Replicate to owner only
	DOREPLIFETIME_CONDITION(UKillTrackingComponent, CurrentStreakCombo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UKillTrackingComponent, ComboEndTime, COND_OwnerOnly);
}

void UKillTrackingComponent::OnKillScored()
{
	CurrentSpree++;

	CurrentStreakCombo++;
	OnStreakComboUpdatedDelegate.Broadcast(CurrentStreakCombo);

	MatchStats.Kills++;
	OnMatchStatsUpdatedDelegate.Broadcast();

	ComboEndTime = GetWorld()->GetTimeSeconds() + KillingStreakFinishTime;
	GetWorld()->GetTimerManager().SetTimer(StreakComboTimerHandle, this, &UKillTrackingComponent::OnStreakTimerFinished, KillingStreakFinishTime);

	// Since this exists on the PlayerState, it's owner will be some sort of controller, and since announcements are clientside only, only look for PlayerControllers
	const auto PC = Cast<AGladiatorPlayerController>(GetOwner()->GetOwner());
	if (PC)
	{
		if (StreakToAnnouncementMap.Contains(CurrentStreakCombo))
		{
			USoundBase* StreakSound = *StreakToAnnouncementMap.Find(CurrentStreakCombo);
			if (StreakSound)
			{
				PC->ClientQueueSound2D(StreakSound);
			}
		}

		if (SpreeToAnnouncementMap.Contains(CurrentSpree))
		{
			USoundBase* SpreeSound = *SpreeToAnnouncementMap.Find(CurrentSpree);
			if (SpreeSound)
			{
				PC->ClientQueueSound2D(SpreeSound);
			}
		}
	}
} 

void UKillTrackingComponent::OnDeathScored()
{
	CurrentSpree = 0;

	CurrentStreakCombo = 0;
	OnStreakComboUpdatedDelegate.Broadcast(0);

	MatchStats.Deaths++;
	OnMatchStatsUpdatedDelegate.Broadcast();
}

void UKillTrackingComponent::OnStreakTimerFinished()
{
	CurrentStreakCombo = 0;
	OnStreakComboUpdatedDelegate.Broadcast(0);
}

void UKillTrackingComponent::OnRep_MatchStats()
{
	OnMatchStatsUpdatedDelegate.Broadcast();
}

void UKillTrackingComponent::OnRep_CurrentStreakCombo()
{
	OnStreakComboUpdatedDelegate.Broadcast(CurrentStreakCombo);
}

float UKillTrackingComponent::GetStreakTimeRemainingAsRatio() const
{
	if (GetWorld())
	{
		return FMath::Max(ComboEndTime - GetWorld()->GetTimeSeconds(), 0.0f) / KillingStreakFinishTime;
	}

	return -1.0f;
}
