// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaGameStateBase.h"
#include "NinjaGameModeBase.h"
#include "NinjaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "UObject/UObjectIterator.h"
#include "NinjaPlayerState.h"

bool ANinjaGameStateBase::CanGameStart()
{
	if (PlayerArray.Num() < 2) {
		return false;
	}

	for (APlayerState* Curr : PlayerArray) {
		ANinjaPlayerState* PState = (ANinjaPlayerState*)Curr;
		if (PState) {
			if (!PState->GetChosenCharacter()) {
				return false;
			}
		}
	}
	return true;
}
