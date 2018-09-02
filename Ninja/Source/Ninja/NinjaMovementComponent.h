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

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector & OldLocation, const FVector & OldVelocity) override;

	FVector AirJumpDirection;

	FVector AttackDashDirection;

	uint8 bWantsToDash : 1;

	void SetAirJumpDirection();

	UFUNCTION(Server, Unreliable, WithValidation, Category = "Ninja Jumping")
		void ServerSetAirJumpDirection(const FVector& Direction);

	void SetAttackDashDirection();

	UFUNCTION(Server, Unreliable, WithValidation, Category = "Ninja Attacking")
		void ServerSetAttackDashDirection(const FVector& Direction);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ninja Attacking")
		float AttackDashPower;

	UFUNCTION(Server, Unreliable, WithValidation, Category = "Ninja Attacking")
		void ServerDoDash();

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

};

class FSavedMove_Ninja : public FSavedMove_Character
{
public:
	FVector SavedAttackDashDirection;

	FVector SavedAirJumpDirection;

	uint8 bSavedWantsToDash : 1;

	typedef FSavedMove_Character Super;

	// Clear saved move properties, so it can be re-used.
	virtual void Clear() override;

	// A byte containing encoded special movement information (jumping, crouching, etc.)
	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	// Called to set up this saved move(when initially created) to make a predictive correction.
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

	// Called before ClientUpdatePosition uses this SavedMove to make a predictive correction
	virtual void PrepMoveFor(class ACharacter* Character) override;
};

class FNetworkPredictionData_Client_Ninja : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_Ninja(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	// Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};