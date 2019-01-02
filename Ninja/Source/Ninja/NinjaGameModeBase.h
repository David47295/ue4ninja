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

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Ninja")
		void BeginRound();

	AActor * ChoosePlayerStart_Implementation(AController * Player) override;
	
	FTimerHandle ActionPhaseTimerHandle;

	void ClearPlayerStartTags();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Ninja Gameflow")
		int32 ActivePlayerId = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ninja")
		TSubclassOf<ANinjaCharacter> BPNinjaCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Standard Ninja")
		int32 RoundLimit;

	UFUNCTION()
		int32 GetCurrentRound() const;

	UFUNCTION()
		void SetCurrentRound(int32 Round);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug")
		bool Debug;
private:
	UPROPERTY()
		int32 CurrentRound;


};
