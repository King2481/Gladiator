// Created by Bruce Crum


#include "Gladiator/UI/GladiatorHUD.h"
#include "Blueprint/UserWidget.h"
#include "Gladiator/UI/ChatBox.h"

AGladiatorHUD::AGladiatorHUD()
{
	ChatBoxClass = nullptr;
	ChatBoxInstance = nullptr;
}

void AGladiatorHUD::BeginPlay()
{
	Super::BeginPlay();

	ConstructWidgets();
}

void AGladiatorHUD::ConstructWidgets()
{
	for (FWidgetCreationInfo WidgetInfo : WidgetsToCreate)
	{
		if (WidgetInfo.WidgetClass)
		{
			const auto Widget = CreateWidget<UUserWidget>(GetOwningPlayerController(), WidgetInfo.WidgetClass);
			if (Widget)
			{
				Widget->AddToViewport(WidgetInfo.ZOrder);
				InstancedWidgets.AddUnique(Widget);
			}
		}
	}

	if (ChatBoxClass)
	{
		ChatBoxInstance = CreateWidget<UChatBox>(GetOwningPlayerController(), ChatBoxClass);
		if (ChatBoxInstance)
		{
			ChatBoxInstance->AddToViewport();
		}
	}
}

void AGladiatorHUD::StartChatInput(const bool bForTeam /* = false */)
{
	if (ChatBoxInstance)
	{
		ChatBoxInstance->StartChatInput(bForTeam);
	}
}

void AGladiatorHUD::OnChatMessageReceived(const FText& Message, AGladiatorPlayerState* SenderPlayerState)
{
	if (ChatBoxInstance)
	{
		ChatBoxInstance->OnChatMessageRecieved(Message);
	}
}
