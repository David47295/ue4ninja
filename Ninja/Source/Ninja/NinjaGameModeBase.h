// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NinjaGameModeBase.generated.h"


class ANinjaCharacter;
class UUserWidget;
/**
 * 
 */
UCLASS()
class NINJA_API ANinjaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ANinjaGameModeBase();

	UFUNCTION(BlueprintCallable, Category="Ninja")
		void BeginRound();

	AActor * ChoosePlayerStart_Implementation(AController * Player) override;
	
	FTimerHandle ActionPhaseTimerHandle;

	void ClearPlayerStartTags();
	
	UFUNCTION(BlueprintCallable, Category = "Ninja Gameflow")
		void EndTurn();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Ninja Gameflow")
		int32 ActivePlayerId = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ninja")
		TSubclassOf<ANinjaCharacter> BPNinjaCharacterClass;

	UFUNCTION()
		void StartActionPhaseTimer(float length);

	UPROPERTY(EditDefaultsOnly, Category="Standard Ninja")
		int32 RoundLimit;

	UFUNCTION()
		int32 GetCurrentRound() const;

	UFUNCTION()
		void SetCurrentRound(int32 Round);

private:
	UPROPERTY()
		int32 CurrentRound;


};
