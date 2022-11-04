// Created by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GladiatorPlayerController.generated.h"

class AGladiatorPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoundWonDelegate, AGladiatorPlayerState*, WinningPlayerState, uint8, WinningTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAnnouncementNoticeDelegate, FText, Notice);

/**
 * 
 */
UCLASS()
class GLADIATOR_API AGladiatorPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AGladiatorPlayerController();

	// Called when we begin play
	virtual void BeginPlay() override;

	// Called when we want to setup input for this player controller.
	virtual void SetupInputComponent() override;

	virtual void ClientTeamMessage_Implementation(APlayerState* SenderPlayerStateBase, const FString& S, FName Type, float MsgLifeTime) override;

	// Called when we recieve a chat message
	virtual void OnChatMessageReceived(const FText& Message, AGladiatorPlayerState* SenderPlayerState = nullptr);

	// Server RPC to send chat messages.
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendChatMessage(const FText& Message, const bool bTeamMessage);
	void ServerSendChatMessage_Implementation(const FText& Message, const bool bTeamMessage);
	bool ServerSendChatMessage_Validate(const FText& Message, const bool bTeamMessage);

	// Called when the chatbox has popped up and text input is allowed
	void OnChatInputStarted();

	// Called when the chatbox has closed.
	void OnChatInputEnded();

	// Sets the pause menu state. 
	UFUNCTION(BlueprintCallable, Category = "Gladiator Player Controller")
	void SetPauseMenuState(const bool bVisible);

	// Sets the scoreboard state. 
	void SetScoreboardState(const bool bVisible);

	// Called when we want to start a chat
	void StartChat();

	// Called when we want to start a team chat
	void StartTeamChat();

	// Called when we want to open the pause menu.
	void OpenPauseMenu();

	// Called when we want to open the scoreboard
	void OpenScoreboard();

	// Called when we want to close the scoreboard
	void CloseScoreboard();

	// Is this player currently inputing text?
	bool bIsChatting;

	// Is this player currently in the pause menu?
	bool bIsInPauseMenu;

	// Is this player currently viewing the scoreboard?
	bool bIsViewingScoreboard;

	// Updates the players input mode, we may want different input modes such as Game / UI / Game & UI
	void UpdateInputMode();

	/* Called when a round has been won from the gamemode.
	* If single based, there will be a winning player state,
	* otherwise, a winning team.
	*/
	UFUNCTION(BlueprintCallable, Category = "Gladiator Player Controller")
	void OnRoundWon(AGladiatorPlayerState* WinningPlayerState, uint8 WinningTeam);

	// Client version of OnRoundWon()
	UFUNCTION(Client, Reliable)
	void ClientOnRoundWon(AGladiatorPlayerState* WinningPlayerState, uint8 WinningTeam);
	void ClientOnRoundWon_Implementation(AGladiatorPlayerState* WinningPlayerState, uint8 WinningTeam);

	// Delegate for when a round is won.
	UPROPERTY(BlueprintAssignable)
	FOnRoundWonDelegate OnRoundWonDelegate;

	// Called to update the announcement panel. Unreliable as this is just a cosmetic thing and don't want to take up bandwidth
	UFUNCTION(Client, Unreliable)
	void ClientRecieveAnnouncementNotice(const FText& Notice);
	void ClientRecieveAnnouncementNotice_Implementation(const FText& Notice);

	// Delegate for when we want to show something on the annoucement panel.
	UPROPERTY(BlueprintAssignable)
	FAnnouncementNoticeDelegate AnnouncementNoticeDelegate;

	// Will play a sound only this player will hear
	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Gladiator Player Controller")
	void ClientPlaySound2D(USoundBase* SoundToPlay);
	void ClientPlaySound2D_Implementation(USoundBase* SoundToPlay);

	// Will play a sound only this player will hear (queue version)
	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Gladiator Player Controller")
	void ClientQueueSound2D(USoundBase* SoundToPlay);
	void ClientQueueSound2D_Implementation(USoundBase* SoundToPlay);

protected:

	// What pause menu class is used.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Gladiator Player Controller")
	TSubclassOf<UUserWidget> PauseMenuClass;

	// The actual instance of the pause menu
	UUserWidget* PauseMenuWidget;

	// What scoreboard class is used.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Gladiator Player Controller")
	TSubclassOf<UUserWidget> ScoreboardClass;

	// The actual instance of the scoreboard
	UUserWidget* ScoreboardWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Gladiator Player Controller")
	TArray<USoundBase*> PendingSoundsToPlay;

	UPROPERTY()
	FTimerHandle SoundQueueTimerHandle;

	void OnQueuedSoundFinished();

};
