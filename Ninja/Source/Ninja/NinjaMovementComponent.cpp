// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaMovementComponent.h"
#include "NinjaCharacter.h"
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
		MoveDirection = PawnOwner->GetLastMovementInputVector();
		AirJumpDirection = MoveDirection * 600.f + FVector(0.f, 0.f, 500.f);
		SetAirJumpDirection();
		if (!bWantsToDash) {
			SetAttackDashDirection(MoveDirection * AttackDashPower);

			UWorld* World = GetWorld();
			if (World) {
				AttackDashCooldownTimer -= World->GetDeltaSeconds();
			}

		}

		if (!bWantsToDodge) {
			DodgeDirection = MoveDirection * DodgePower;
		}

		if (bWantsToDodge) {
			DodgeTimeline.TickTimeline(GetWorld()->GetDeltaSeconds());
			DodgeTimelineValue = DodgeSpeedCurve->GetFloatValue(DodgeTimeline.GetPlaybackPosition());
		}
	}

	if (PawnOwner->Role < ROLE_Authority) {
		ServerSetMoveDirection(MoveDirection);
		Server_SetDodgeTimelineValue(DodgeTimelineValue);

	}

	DoDash();

	DoDodge();
}

void UNinjaMovementComponent::ServerSetMoveDirection_Implementation(const FVector & Dir) {
	MoveDirection = Dir;

	AirJumpDirection = MoveDirection * 600.f + FVector(0.f, 0.f, 500.f);

	if (!bWantsToDash) {
		AttackDashDirection = MoveDirection * AttackDashPower;
	}

	if (!bWantsToDodge) {
		DodgeDirection = MoveDirection * DodgePower;
	}
	
}

bool UNinjaMovementComponent::ServerSetMoveDirection_Validate(const FVector & Dir) {
	return true;
}

void UNinjaMovementComponent::SetAirJumpDirection() {
	if (PawnOwner->IsLocallyControlled()) {
		FVector MovementInput = PawnOwner->GetLastMovementInputVector();
		AirJumpDirection = MovementInput * 600.f + FVector(0.f, 0.f, 500.f);
	}
}

void UNinjaMovementComponent::SetAttackDashDirection(const FVector & Direction)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("X: %f Y: %f Z: %f"),Dir.X,  Dir.Y, Dir.Z));
	AttackDashDirection = Direction;

}

void UNinjaMovementComponent::ServerSetAttackDashDirection_Implementation(const FVector & Direction)
{
	if (!bWantsToDash) {
		AttackDashDirection = Direction;
	}
}

bool UNinjaMovementComponent::ServerSetAttackDashDirection_Validate(const FVector & Direction)
{
	return true;
}

void UNinjaMovementComponent::ServerSetAirJumpDirection_Implementation(const FVector& Direction)
{
	AirJumpDirection = Direction;
}

bool UNinjaMovementComponent::ServerSetAirJumpDirection_Validate(const FVector& Direction)
{
	return true;
}


void UNinjaMovementComponent::DoDash()
{
	ANinjaCharacter* Char = (ANinjaCharacter*)PawnOwner;
	if (bWantsToDash && AttackDashDirection != FVector::ZeroVector) {
		Velocity = AttackDashDirection;

		//if (PawnOwner->IsLocallyControlled()) {
		//	ServerDoDash();
		//}
	}

}

void UNinjaMovementComponent::ServerDoDash_Implementation()
{
	ANinjaCharacter* Char = (ANinjaCharacter*)PawnOwner;
	if (bWantsToDash && AttackDashDirection != FVector::ZeroVector) {
		Launch(AttackDashDirection);
	}
}

bool UNinjaMovementComponent::ServerDoDash_Validate()
{
	return true;
}

void UNinjaMovementComponent::SetAttackDashCooldownTimer_Implementation(float Time) {

}

bool UNinjaMovementComponent::SetAttackDashCooldownTimer_Validate(float Time) {
	return true;
}


void UNinjaMovementComponent::DoDodge()
{
	UWorld* World = GetWorld();
	if (World) {
		ANinjaCharacter* Char = (ANinjaCharacter*)PawnOwner;
		if (Char) {
			if (bWantsToDodge) {
				Velocity = DodgeDirection * DodgeTimelineValue;
			}
		}
	}


}

void UNinjaMovementComponent::Dash()
{
	if (!bWantsToDash) {
		bWantsToDash = true;
		AttackDashCooldownTimer = AttackDashCooldown;
	}
	
}

void UNinjaMovementComponent::Dodge()
{
	bWantsToDodge = true;

	FOnTimelineFloat TimelineCallback;
	FOnTimelineEventStatic OnTimelineFinished;
	OnTimelineFinished.BindUFunction(this, FName{ TEXT("StopDodge") });

	DodgeTimeline.AddInterpFloat(DodgeSpeedCurve, NULL);
	DodgeTimeline.SetTimelineFinishedFunc(OnTimelineFinished);
	DodgeTimeline.PlayFromStart();
	
}

void UNinjaMovementComponent::StopDash()
{
	bWantsToDash = false;
}

void UNinjaMovementComponent::StopDodge()
{
	bWantsToDodge = false;

	ANinjaCharacter* Char = (ANinjaCharacter*)PawnOwner;
	if (Char) {
		Char->SetIsDodging(false);
		if (PawnOwner->Role == ROLE_AutonomousProxy) {
			Char->Server_SetIsDodging(false);
		}
		
	}
}

void UNinjaMovementComponent::Server_SetDodgeTimelineValue_Implementation(float Value)
{
	DodgeTimelineValue = Value;
}

bool UNinjaMovementComponent::Server_SetDodgeTimelineValue_Validate(float Value)
{
	return true;
}

bool UNinjaMovementComponent::GetWantsToDodge() const
{
	return bWantsToDodge;
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

void UNinjaMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToDodge = (Flags&FSavedMove_Character::FLAG_Custom_0) != 0;
	bWantsToDash = (Flags&FSavedMove_Character::FLAG_Custom_0) != 0;
}

void FSavedMove_Ninja::Clear()
{
	Super::Clear();

	SavedAirJumpDirection = FVector::ZeroVector;
	SavedAttackDashDirection = FVector::ZeroVector;
	bSavedWantsToDodge = false;
	bSavedWantsToDash = false;
	SavedDodgeTimelineValue = 0.f;
	SavedAttackDashCooldownTimer = 0.f;

}

uint8 FSavedMove_Ninja::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedWantsToDodge) {
		Result |= FLAG_Custom_0;
	}
	else if (bSavedWantsToDash) {
		Result |= FLAG_Custom_0;
	}

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
		SavedAttackDashDirection = CharMov->AttackDashDirection;
		bSavedWantsToDodge = CharMov->bWantsToDodge;
		bSavedWantsToDash = CharMov->bWantsToDash;
		SavedDodgeTimelineValue = CharMov->DodgeTimelineValue;
		SavedAttackDashCooldownTimer = CharMov->AttackDashCooldownTimer;
	}
}

void FSavedMove_Ninja::PrepMoveFor(ACharacter * Character)
{
	Super::PrepMoveFor(Character);
	UNinjaMovementComponent* CharMov = (UNinjaMovementComponent*)Character->GetCharacterMovement();
	if (CharMov) {
		CharMov->AirJumpDirection = SavedAirJumpDirection;
		CharMov->AttackDashDirection = SavedAttackDashDirection;
		CharMov->bWantsToDodge = bSavedWantsToDodge;
		CharMov->bWantsToDash = bSavedWantsToDash;
		CharMov->DodgeTimelineValue = SavedDodgeTimelineValue;
		CharMov->AttackDashCooldownTimer = SavedAttackDashCooldownTimer;
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


