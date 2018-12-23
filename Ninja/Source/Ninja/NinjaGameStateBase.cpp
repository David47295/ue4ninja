// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaGameStateBase.h"
#include "NinjaGameModeBase.h"
#include "NinjaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "UserWidget.h"

void ANinjaGameStateBase::ShowEndScreen_Implementation()
{

	if (Role < ROLE_Authority) {
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("Client: GAME OVER"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("GAME OVER"));
	}
	
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0);
		if (Controller) {
			//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, FString::Printf(TEXT("%s"), *Controller->GetName()));
			
		}
	}
	
}
