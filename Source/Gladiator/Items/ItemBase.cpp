// Created by Bruce Crum


#include "Gladiator/Items/ItemBase.h"

DEFINE_LOG_CATEGORY(LogItem);

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	ItemName = FText::GetEmpty();
	ItemSlot = nullptr;
}

void AItemBase::InitItem(AActor* NewOwner)
{
	SetOwner(NewOwner);
	SetInstigator(Cast<APawn>(NewOwner));
}