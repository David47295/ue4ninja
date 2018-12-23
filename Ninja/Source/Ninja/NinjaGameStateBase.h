// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NinjaGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class NINJA_API ANinjaGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	// 0 for Planning phase. 1 for Action phase
	int32 GamePhase = 0;
	
	UPROPERTY(EditDefaultsOnly, Category="Ninja Widgets")
		TSubclassOf<UUserWidget> EndScreenWidget;
};
