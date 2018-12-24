// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaGameStateBase.h"
#include "NinjaGameModeBase.h"
#include "NinjaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "UObject/UObjectIterator.h"

void ANinjaGameStateBase::AllClientsRemoveAllWidgets_Implementation()
{
	UWorld* World = GetWorld();
	if (World) {
		for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
		{
			UUserWidget* Widget = *Itr;

			if (Widget->GetWorld())
			{
				Widget->RemoveFromParent();
			}
		}
	}

	APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0);
	if (Controller) {
		FInputModeGameOnly Mode;
		Controller->bShowMouseCursor = false;
		Controller->SetInputMode(Mode);
	}
}
