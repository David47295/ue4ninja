// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaMovementComponent.h"
#include "Engine.h"

bool UNinjaMovementComponent::DoJump(bool bReplayingMoves) {
	if (IsFalling()) {

		//SetAirJumpDirection();

		Launch(AirJumpDirection);
		return true;
	}
	Super::DoJump(bReplayingMoves);
	return true;
}

void UNinjaMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector & OldLocation, const FVector & OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (!PawnOwner) { return; }

	if (PawnOwner->IsLocallyControlled()) {
		FVector MovementInput = PawnOwner->GetLastMovementInputVector();
		AirJumpDirection = MovementInput * 600.f + FVector(0.f, 0.f, 500.f);
	}

	if (PawnOwner->Role < ROLE_Authority) {
		FVector MovementInput = PawnOwner->GetLastMovementInputVector();
		ServerSetAirJumpDirection(MovementInput * 600.f + FVector(0.f, 0.f, 500.f));
	}

}

void UNinjaMovementComponent::SetAirJumpDirection() {
	FVector MovementInput = PawnOwner->GetLastMovementInputVector();
	AirJumpDirection = MovementInput * 600.f + FVector(0.f, 0.f, 500.f);

	if (PawnOwner->Role < ROLE_Authority) {
		ServerSetAirJumpDirection(AirJumpDirection);
	}
}

void UNinjaMovementComponent::ServerSetAirJumpDirection_Implementation(const FVector& Direction)
{
	AirJumpDirection = Direction;

}

bool UNinjaMovementComponent::ServerSetAirJumpDirection_Validate(const FVector& Direction)
{
	return true;
}

FNetworkPredictionData_Client* UNinjaMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);
	if (PawnOwner->Role < ROLE_Authority)
	{
		if (!ClientPredictionData)
		{
			UNinjaMovementComponent* MutableThis = const_cast<UNinjaMovementComponent*>(this);

			MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Ninja(*this);
			MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
			MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
		}

		return ClientPredictionData;
	}
	return nullptr;
}

void FSavedMove_Ninja::Clear()
{
	Super::Clear();

	SavedAirJumpDirection = FVector::ZeroVector;
}

uint8 FSavedMove_Ninja::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	return Result;
}

bool FSavedMove_Ninja::CanCombineWith(const FSavedMovePtr & NewMove, ACharacter * Character, float MaxDelta) const
{
	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_Ninja::SetMoveFor(ACharacter * Character, float InDeltaTime, FVector const & NewAccel, FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UNinjaMovementComponent* CharMov = (UNinjaMovementComponent*)Character->GetCharacterMovement();
	if (CharMov) {
		SavedAirJumpDirection = CharMov->AirJumpDirection;
	}
}

void FSavedMove_Ninja::PrepMoveFor(ACharacter * Character)
{
	Super::PrepMoveFor(Character);
	UNinjaMovementComponent* CharMov = (UNinjaMovementComponent*)Character->GetCharacterMovement();
	if (CharMov) {
		CharMov->AirJumpDirection = SavedAirJumpDirection;
	}
}

FNetworkPredictionData_Client_Ninja::FNetworkPredictionData_Client_Ninja(const UCharacterMovementComponent & ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_Ninja::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Ninja());
}


