// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItem, Log, Verbose);

class UItemSlot;

UCLASS()
class GLADIATOR_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

	// Initializes the item with actors that "owns" this.
	virtual void InitItem(AActor* NewOwner);

	// What is this items name?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item Base")
	FText ItemName;

	// What slot does this item go to?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item Base")
	UItemSlot* ItemSlot;
};
