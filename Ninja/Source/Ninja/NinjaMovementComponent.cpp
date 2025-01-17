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
		UWorld* World = GetWorld();
		if (World) {
			MoveDirection = PawnOwner->GetLastMovementInputVector();
			AirJumpDirection = MoveDirection * AirJumpPower + FVector(0.f, 0.f, AirJumpPower);
			SetAirJumpDirection();

			if (!bWantsToDash) {
				SetAttackDashDirection(MoveDirection * AttackDashPower);
				
			}

			if (!bWantsToDodge) {
				DodgeDirection = MoveDirection * DodgePower;
				
			}

			if (bWantsToDodge) {
				DodgeTimeline.TickTimeline(GetWorld()->GetDeltaSeconds());
				DodgeTimelineValue = DodgeSpeedCurve->GetFloatValue(DodgeTimeline.GetPlaybackPosition());
			}

			AttackDashCooldownTimer -= World->GetDeltaSeconds();
			DodgeCooldownTimer -= World->GetDeltaSeconds();
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

	AirJumpDirection = MoveDirection * AirJumpPower + FVector(0.f, 0.f, AirJumpPower);

	if (!bWantsToDash) {
		AttackDashDirection = MoveDirection * AttackDashPower;
	}

	if (!bWantsToDodge) {
		DodgeDirection = MoveDirection * DodgePower;
	}
	
	UWorld* World = GetWorld();
	if (World) {
		AttackDashCooldownTimer -= World->GetDeltaSeconds();
		DodgeCooldownTimer -= World->GetDeltaSeconds();
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

void UNinjaMovementComponent::Server_SetAttackDashCooldownTimer_Implementation(float Time) {
	AttackDashCooldownTimer = Time;
}

bool UNinjaMovementComponent::Server_SetAttackDashCooldownTimer_Validate(float Time) {
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
	if (!bWantsToDash && !bWantsToDodge) {
		bWantsToDash = true;
		AttackDashCooldownTimer = AttackDashCooldown;

		if (PawnOwner->Role == ROLE_AutonomousProxy) {
			Server_SetAttackDashCooldownTimer(AttackDashCooldown);
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("Can't Dash")));
	}
	
}

void UNinjaMovementComponent::Dodge()
{

	if (!bWantsToDodge && !bWantsToDash && DodgeCooldownTimer <= 0.f) {
		bWantsToDodge = true;
		DodgeCooldownTimer = DodgeCooldown;

		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic OnTimelineFinished;
		OnTimelineFinished.BindUFunction(this, FName{ TEXT("StopDodge") });

		DodgeTimeline.AddInterpFloat(DodgeSpeedCurve, NULL);
		DodgeTimeline.SetTimelineFinishedFunc(OnTimelineFinished);
		DodgeTimeline.PlayFromStart();
	}
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
	bWantsToDash = (Flags&FSavedMove_Character::FLAG_Custom_1) != 0;
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
	if (bSavedWantsToDash) {
		Result |= FLAG_Custom_1;
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
		SavedDodgeCooldownTimer = CharMov->DodgeCooldownTimer;
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
		CharMov->DodgeCooldownTimer = SavedDodgeCooldownTimer;
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


