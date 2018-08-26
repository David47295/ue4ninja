// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaGameModeBase.h"
#include "NinjaCharacter.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"


ANinjaGameModeBase::ANinjaGameModeBase() {
	DefaultPawnClass = ANinjaCharacter::StaticClass();
}

void ANinjaGameModeBase::BeginRound()
{
	UWorld* World = GetWorld();
	if (World) {
		for (TActorIterator<APlayerController> Itr(World); Itr; ++Itr) {
			APlayerController* Cont = *Itr;
			if (Cont) {
				ACharacter* lol = Cont->GetCharacter();
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, TEXT("nut"));
			}
		}
	}
}

AActor * ANinjaGameModeBase::ChoosePlayerStart_Implementation(AController * Player)
{
	UWorld* World = GetWorld();
	if (World) {
		for (TActorIterator<APlayerStart> Itr(World); Itr; ++Itr) {
			APlayerStart* Actor = *Itr;
			if (Actor) {
				if (!(Actor->PlayerStartTag.IsEqual(FName("Taken")))) {
					return Actor;
				}
			}
		}
	}
	return nullptr;
}


