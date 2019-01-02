// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaGameModeBase.h"
#include "NinjaCharacter.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Blueprint/UserWidget.h"
#include "NinjaPlayerState.h"
#include "NinjaGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "NinjaGameInstance.h"


ANinjaGameModeBase::ANinjaGameModeBase() {

}

void ANinjaGameModeBase::BeginPlay()
{
	UNinjaGameInstance* GameInst = (UNinjaGameInstance*)GetGameInstance();
	if (GameInst && !Debug) {
		RoundLimit = GameInst->GetNumRounds();
	}
}

void ANinjaGameModeBase::BeginRound()
{
	UWorld* World = GetWorld();
	if (World) {
		ClearPlayerStartTags();
		for (TActorIterator<APlayerController> Itr(World); Itr; ++Itr) {
			APlayerController* Cont = *Itr;
			if (Cont) {
				ACharacter* Character = Cont->GetCharacter();
				APlayerStart* PS = (APlayerStart*)ChoosePlayerStart(Cont);
				if (PS) {
					PS->PlayerStartTag = FName(TEXT("Taken"));
					if (Character) {
						Character->Destroy();
					}

					FActorSpawnParameters Params;
					Params.Owner = Cont;
					ANinjaCharacter* SpawnedChar = World->SpawnActor<ANinjaCharacter>(BPNinjaCharacterClass, PS->GetActorLocation(), PS->GetActorRotation(), Params);
					if (SpawnedChar) {
						Cont->Possess(SpawnedChar);
					}
				}

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
				if (Actor->PlayerStartTag.IsEqual(FName(TEXT("None")))) {
					//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("Tag is None"));
					return Super::ChoosePlayerStart_Implementation(Player);
				}

				if (!(Actor->PlayerStartTag.IsEqual(FName("Taken")))) {
					//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("PS is not taken"));
					return Actor;
				}
			}
		}
	}
	return nullptr;
}

void ANinjaGameModeBase::ClearPlayerStartTags() {
	UWorld* World = GetWorld();
	if (World) {
		for (TActorIterator<APlayerStart> Itr(World); Itr; ++Itr) {
			APlayerStart* Actor = *Itr;
			if (Actor) {
				Actor->PlayerStartTag = FName(TEXT("Free"));
			}
		}
	}
}

int32 ANinjaGameModeBase::GetCurrentRound() const
{
	return CurrentRound;
}

void ANinjaGameModeBase::SetCurrentRound(int32 Round)
{
	CurrentRound = Round;
}

