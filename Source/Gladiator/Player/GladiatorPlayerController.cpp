// Created by Bruce Crum


#include "Gladiator/Player/GladiatorPlayerController.h"
#include "Gladiator/Player/GladiatorPlayerState.h"
#include "Gladiator/UI/GladiatorHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AGladiatorPlayerController::AGladiatorPlayerController()
{
	bIsChatting = false;
	bIsInPauseMenu = false;
	bIsViewingScoreboard = false;

	PauseMenuClass = nullptr;
	PauseMenuWidget = nullptr;

	ScoreboardClass = nullptr;
	ScoreboardWidget = nullptr;
}

void AGladiatorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		{
			if (PauseMenuClass)
			{
				const auto Widget = CreateWidget<UUserWidget>(this, PauseMenuClass);
				if (Widget)
				{
					Widget->AddToViewport(100);
					Widget->SetVisibility(ESlateVisibility::Collapsed);
					PauseMenuWidget = Widget;
				}
			}
		}

		{
			if (ScoreboardClass)
			{
				const auto Widget = CreateWidget<UUserWidget>(this, ScoreboardClass);
				if (Widget)
				{
					Widget->AddToViewport(100);
					Widget->SetVisibility(ESlateVisibility::Collapsed);
					ScoreboardWidget = Widget;
				}
			}
		}
	}

}

void AGladiatorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("StartChat", IE_Pressed, this, &AGladiatorPlayerController::StartChat);
	InputComponent->BindAction("StartTeamChat", IE_Pressed, this, &AGladiatorPlayerController::StartTeamChat);

	InputComponent->BindAction("OpenPauseMenu", IE_Pressed, this, &AGladiatorPlayerController::OpenPauseMenu);

	InputComponent->BindAction("OpenScoreboard", IE_Pressed, this, &AGladiatorPlayerController::OpenScoreboard);
	InputComponent->BindAction("OpenScoreboard", IE_Released, this, &AGladiatorPlayerController::CloseScoreboard);
}

void AGladiatorPlayerController::StartChat()
{
	const auto GladiatorHUD = Cast<AGladiatorHUD>(GetHUD());
	if (GladiatorHUD)
	{
		GladiatorHUD->StartChatInput();
	}
}

void AGladiatorPlayerController::StartTeamChat()
{
	const auto GladiatorHUD = Cast<AGladiatorHUD>(GetHUD());
	if (GladiatorHUD)
	{
		GladiatorHUD->StartChatInput(true);
	}
}

void AGladiatorPlayerController::OpenPauseMenu()
{
	SetPauseMenuState(true);
}

void AGladiatorPlayerController::OpenScoreboard()
{
	SetScoreboardState(true);
}

void AGladiatorPlayerController::CloseScoreboard()
{
	SetScoreboardState(false);
}

void AGladiatorPlayerController::ServerSendChatMessage_Implementation(const FText& Message, const bool bTeamMessage)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		const auto PC = Cast<AGladiatorPlayerController>(*Iterator);
		if (!PC)
		{
			continue;
		}

		PC->ClientTeamMessage(PlayerState, Message.ToString(), bTeamMessage ? TEXT("Team") : TEXT(""));
	}
}

bool AGladiatorPlayerController::ServerSendChatMessage_Validate(const FText& Message, const bool bTeamMessage)
{
	return true;
}

void AGladiatorPlayerController::ClientTeamMessage_Implementation(APlayerState* SenderPlayerStateBase, const FString& S, FName Type, float MsgLifeTime)
{
	Super::ClientTeamMessage_Implementation(SenderPlayerStateBase, S, Type, MsgLifeTime);

	const auto SenderPlayerState = Cast<AGladiatorPlayerState>(SenderPlayerStateBase);;

	const bool bGamemodeSay = Type == FName(TEXT("Gamemode"));
	const bool bHostSay = Type == FName(TEXT("Host"));
	const bool bTeamSay = Type == FName(TEXT("Team"));

	static FFormatNamedArguments Arguments;
	Arguments.Add(TEXT("Name"), FText::FromString(SenderPlayerState ? SenderPlayerState->GetPlayerName() : TEXT("")));

	if (bGamemodeSay)
	{
		Arguments.Add(TEXT("Title"), FText::FromString(TEXT("")));
	}
	else
	{
		Arguments.Add(TEXT("Title"), FText::FromString(bTeamSay ? TEXT("(Team):") : TEXT("(All):")));
	}

	Arguments.Add(TEXT("Message"), FText::FromString(S));

	OnChatMessageReceived(FText::Format(NSLOCTEXT("HUD", "ChatMessageFormat", "{Name} {Title} {Message}"), Arguments), SenderPlayerState);
}

void AGladiatorPlayerController::OnChatMessageReceived(const FText& Message, AGladiatorPlayerState* SenderPlayerState /*= nullptr*/)
{
	const auto GladiatorHUD = Cast<AGladiatorHUD>(GetHUD());
	if (GladiatorHUD)
	{
		GladiatorHUD->OnChatMessageReceived(Message, SenderPlayerState);
	}
}

void AGladiatorPlayerController::OnChatInputStarted()
{
	bIsChatting = true;
	UpdateInputMode();
}

void AGladiatorPlayerController::OnChatInputEnded()
{
	bIsChatting = false;
	UpdateInputMode();
}

void AGladiatorPlayerController::SetPauseMenuState(const bool bVisible)
{
	if (PauseMenuWidget)
	{
		bIsInPauseMenu = bVisible;
		PauseMenuWidget->SetVisibility(bIsInPauseMenu ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		UpdateInputMode();
	}
}

void AGladiatorPlayerController::SetScoreboardState(const bool bVisible)
{
	if (ScoreboardWidget)
	{
		bIsViewingScoreboard = bVisible;
		ScoreboardWidget->SetVisibility(bIsViewingScoreboard ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		UpdateInputMode();
	}
}

void AGladiatorPlayerController::UpdateInputMode()
{
	if (bIsChatting || bIsInPauseMenu)
	{
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
		return;
	}

	if (bIsViewingScoreboard)
	{
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
		return;
	}

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void AGladiatorPlayerController::OnRoundWon(AGladiatorPlayerState* WinningPlayerState, uint8 WinningTeam)
{
	ClientOnRoundWon(WinningPlayerState, WinningTeam);
}

void AGladiatorPlayerController::ClientOnRoundWon_Implementation(AGladiatorPlayerState* WinningPlayerState, uint8 WinningTeam)
{
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	OnRoundWonDelegate.Broadcast(WinningPlayerState, WinningTeam);
}

void AGladiatorPlayerController::ClientRecieveAnnouncementNotice_Implementation(const FText& Notice)
{
	AnnouncementNoticeDelegate.Broadcast(Notice);
}

void AGladiatorPlayerController::ClientPlaySound2D_Implementation(USoundBase* SoundToPlay)
{
	if (SoundToPlay)
	{
		UGameplayStatics::PlaySound2D(this, SoundToPlay);
	}
}

void AGladiatorPlayerController::ClientQueueSound2D_Implementation(USoundBase* SoundToPlay)
{
	if (SoundToPlay)
	{
		if (GetWorldTimerManager().IsTimerActive(SoundQueueTimerHandle))
		{
			PendingSoundsToPlay.Add(SoundToPlay);
		}
		else
		{
			UGameplayStatics::PlaySound2D(this, SoundToPlay);
			GetWorldTimerManager().SetTimer(SoundQueueTimerHandle, this, &AGladiatorPlayerController::OnQueuedSoundFinished, SoundToPlay->GetDuration());
		}
	}
}

void AGladiatorPlayerController::OnQueuedSoundFinished()
{
	// TODO: Cleanup
	if (PendingSoundsToPlay.Num() > 0)
	{
		UGameplayStatics::PlaySound2D(this, PendingSoundsToPlay[0]);
		GetWorldTimerManager().SetTimer(SoundQueueTimerHandle, this, &AGladiatorPlayerController::OnQueuedSoundFinished, PendingSoundsToPlay[0]->GetDuration());
		PendingSoundsToPlay.RemoveAt(0);
	}
}