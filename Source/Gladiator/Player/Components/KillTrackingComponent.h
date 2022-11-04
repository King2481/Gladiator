// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KillTrackingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchStatsUpdatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStreakComboUpdatedDelegate, int32, NewStreakCombo);

USTRUCT(BlueprintType)
struct FMatchStats
{
	GENERATED_BODY()

	// Kills Scored
	UPROPERTY(BlueprintReadOnly)
	int32 Kills;

	// Deaths Scored
	UPROPERTY(BlueprintReadOnly)
	int32 Deaths;

	FMatchStats()
	{
		Kills = 0;
		Deaths = 0;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLADIATOR_API UKillTrackingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UKillTrackingComponent();

	// Called when we score a kill.
	void OnKillScored();

	// Called when we score a death.
	void OnDeathScored();

	// How long does it take for a killing streak to end?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Kill Tracking Component")
	float KillingStreakFinishTime;

	UFUNCTION(BlueprintPure, Category = "Kill Tracking Component")
	float GetStreakTimeRemainingAsRatio() const;

private:

	// What is the current spree of this component?
	int32 CurrentSpree;

	// Called when StreakComboManager finishes it's time
	void OnStreakTimerFinished();

	FTimerHandle StreakComboTimerHandle;

protected:

	// Function for replication setup.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Match stats for this player.
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_MatchStats, Category = "Kill Tracking Component")
	FMatchStats MatchStats;

	UFUNCTION()
	void OnRep_MatchStats();

	UPROPERTY(BlueprintAssignable)
	FOnMatchStatsUpdatedDelegate OnMatchStatsUpdatedDelegate;

	// What is the current streak combo? (Different than sprees, which tracks how much you've killed total, not just recently)
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_CurrentStreakCombo, Category = "Kill Tracking Component")
	int32 CurrentStreakCombo;

	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_CurrentStreakCombo, Category = "Kill Tracking Component")
	float ComboEndTime;

	UFUNCTION()
	void OnRep_CurrentStreakCombo();

	UPROPERTY(BlueprintAssignable)
	FOnStreakComboUpdatedDelegate OnStreakComboUpdatedDelegate;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Kill Tracking Component")
	TMap<int32, USoundBase*> SpreeToAnnouncementMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Kill Tracking Component")
	TMap<int32, USoundBase*> StreakToAnnouncementMap;
};
