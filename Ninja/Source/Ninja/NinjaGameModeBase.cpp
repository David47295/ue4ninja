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
		ClearPlayerStartTags();
		for (TActorIterator<APlayerController> Itr(World); Itr; ++Itr) {
			APlayerController* Cont = *Itr;
			if (Cont) {
				ACharacter* Character = Cont->GetCharacter();
				APlayerStart* PS = (APlayerStart*)ChoosePlayerStart(Cont);
				if (PS) {
					
					PS->PlayerStartTag = FName(TEXT("Taken"));
					//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("Tag: %s"), *PS->PlayerStartTag.ToString()));
					Character->SetActorLocation(PS->GetActorLocation());
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
