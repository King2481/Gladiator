// Created by Bruce Crum


#include "Gladiator/GameFramework/GladiatorGameInstance.h"

UGladiatorGameInstance::UGladiatorGameInstance()
{

}

void UGladiatorGameInstance::AddMapCycleEntry(const FMapCycleEntry& NewEntry)
{
	MapCycle.Add(NewEntry);
}