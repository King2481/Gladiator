// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GladiatorHUD.generated.h"

class UChatBox;
class AGladiatorPlayerState;

USTRUCT(BlueprintType)
struct FWidgetCreationInfo
{
	GENERATED_BODY()

	/* The Widget class we are trying to create. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<UUserWidget> WidgetClass;

	/* The widgets specified Z-Order. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 ZOrder;
};

/**
 * 
 */
UCLASS()
class GLADIATOR_API AGladiatorHUD : public AHUD
{
	GENERATED_BODY()

public:

	AGladiatorHUD();

	/* Called when the actor begins play. */
	virtual void BeginPlay() override;

	// Called when we recieved a message;
	virtual void OnChatMessageReceived(const FText& Message, AGladiatorPlayerState* SenderPlayerState);

	// Begins chat input
	void StartChatInput(const bool bForTeam = false);

protected:

	/* What widgets are created for this HUD? */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Gladiator HUD")
	TArray<FWidgetCreationInfo> WidgetsToCreate;

	/* Constructs any widget this HUD might have. */
	virtual void ConstructWidgets();

	/* An array of instanced widgets this HUD has. */
	TArray<UUserWidget*> InstancedWidgets;

	/* What chatbox widget class is used? */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Gladiator HUD")
	TSubclassOf<UChatBox> ChatBoxClass;

	/* Pointer to the chatbox instance for this HUD */
	UChatBox* ChatBoxInstance;
};
