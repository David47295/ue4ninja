// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NinjaPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NINJA_API ANinjaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "NinjaCharacterSelect")
		TSubclassOf<ACharacter> GetChosenCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "NinjaCharacterSelect")
		void SetChosenCharacter(TSubclassOf<ACharacter> Character);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetChosenCharacter(TSubclassOf<ACharacter> Character);
	
protected:
	TSubclassOf<ACharacter> ChosenCharacter;
};
