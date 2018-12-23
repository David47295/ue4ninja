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


ANinjaGameModeBase::ANinjaGameModeBase() {
	//DefaultPawnClass = ANinjaCharacter::StaticClass();
}

void ANinjaGameModeBase::BeginRound()
{
	UWorld* World = GetWorld();
	if (World) {
		ClearPlayerStartTags();
		for (TActorIterator<APlayerController> Itr(World); Itr; ++Itr) {
			APlayerController* Cont = *Itr;
			if (Cont) {
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("Controller"));
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
		ANinjaGameStateBase* GS = (ANinjaGameStateBase*)GameState;
		if (GS) {
			ANinjaPlayerState* PState = (ANinjaPlayerState*)GS->PlayerArray[0];
			if (PState) {
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("Setting IsmyTurn"));
				GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Blue, FString::Printf(TEXT("Value: %s"), *PState->GetOwner()->GetName()));
				PState->bIsMyTurn = true;
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

void ANinjaGameModeBase::EndTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("Turn Over!"));
	UWorld* World = GetWorld();
	if (World) {
		ANinjaGameStateBase* GS = (ANinjaGameStateBase*)GameState;
		if (GS) {
			ANinjaPlayerState* PState = (ANinjaPlayerState*)GS->PlayerArray[ActivePlayerId];
			if (PState) {
				PState->bIsMyTurn = false;

				if (ActivePlayerId == 0) {
					ActivePlayerId = 1;
				}
				else {
					ActivePlayerId = 0;
				}

				PState = (ANinjaPlayerState*)GS->PlayerArray[ActivePlayerId];
				PState->bIsMyTurn = true;

				AController* Controller = (AController*)PState->GetOwner();
				if (Controller) {
					ANinjaCharacter* Char = (ANinjaCharacter*)Controller->GetPawn();
					if (Char) {
						Char->SetWorldTime_Server(1.f);
					}
				}
			}
		}
	}
}

void ANinjaGameModeBase::StartActionPhaseTimer(float length)
{
	UWorld* World = GetWorld();
	if (World) {
		World->GetTimerManager().SetTimer(ActionPhaseTimerHandle, this, &ANinjaGameModeBase::EndTurn, length, false);
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

//void ANinjaGameModeBase::PostLogin(APlayerController * NewPlayer)
//{
//	if (GetNumPlayers() == 2) {
//		
//	}
//}
