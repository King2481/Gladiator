// Created by Bruce Crum


#include "Gladiator/UI/UIGameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Gladiator/UI/GladiatorUISettings.h"
#include "Gladiator/UI/KillFeedDisplayTextCollection.h"
#include "Gladiator/UI/KillFeedDisplayText.h"

FText UUIGameplayStatics::GenerateKillFeedTextFromContexts(const UDamageType* InDamageType, APlayerState* KillerPS, APlayerState* VictimPS)
{
	if (InDamageType && KillerPS && VictimPS)
	{
		const auto UISettings = GetDefault<UGladiatorUISettings>();
		if(UISettings)
		{
			UKillFeedDisplayTextCollection* Collection = UISettings->KillFeedDisplayCollection.LoadSynchronous();
			if (Collection)
			{
				const bool bSelfKill = KillerPS == VictimPS;
				
				UKillFeedDisplayText* DisplayText = *Collection->DamageTypeToTextMap.Find(InDamageType->GetClass());
				if (DisplayText)
				{
					int32 Option = FMath::RandRange(0, bSelfKill ? DisplayText->PossibleSelfKillOptions.Num() - 1 : DisplayText->PossibleOptions.Num() - 1);
					const FText SelectedOption = bSelfKill ? DisplayText->PossibleSelfKillOptions[Option] : DisplayText->PossibleOptions[Option];

					FFormatNamedArguments Arguments;
					Arguments.Add(TEXT("KillerPS"), FText::FromString(KillerPS->GetPlayerName()));
					Arguments.Add(TEXT("VictimPS"), FText::FromString(VictimPS->GetPlayerName()));

					return FText::Format(SelectedOption, Arguments);
				}
			}
		}
	}

	return FText::GetEmpty();
}