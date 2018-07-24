// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NinjaMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class NINJA_API UNinjaMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	virtual bool DoJump(bool bReplayingMoves) override;
	
	
};
