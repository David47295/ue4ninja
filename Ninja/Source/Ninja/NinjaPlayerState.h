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
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const override;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_IsMyTurn)
		bool bIsMyTurn;

	UFUNCTION(Server, Reliable, WithValidation)
		void OnRep_IsMyTurn();

};
