// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GladiatorGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FMapCycleEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FName MapName;

	UPROPERTY(BlueprintReadWrite)
	FName ModeName;

	UPROPERTY(BlueprintReadWrite)
	float MatchTime;

	UPROPERTY(BlueprintReadWrite)
	int32 ScoreNeededToWin;

	UPROPERTY(BlueprintReadWrite)
	bool KillFeed;
};

/**
 * 
 */
UCLASS()
class GLADIATOR_API UGladiatorGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UGladiatorGameInstance();
	
	UFUNCTION(BlueprintCallable, Category = "Gladiator Game Instance")
	void AddMapCycleEntry(const FMapCycleEntry& NewEntry);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Gladiator Game Instance")
	TArray<FMapCycleEntry> MapCycle;
};
