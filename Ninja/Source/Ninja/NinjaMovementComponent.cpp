// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaMovementComponent.h"
#include "Engine.h"

bool UNinjaMovementComponent::DoJump(bool bReplayingMoves) {
	if (IsFalling()) {
		float Right = GetPawnOwner()->GetInputAxisValue("MoveRight");
		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("Right: %f"), Right));
		if (Right != 0.f) {
			
			Velocity = GetPawnOwner()->GetActorRightVector() * 600.f * Right;
			Super::DoJump(bReplayingMoves);
			return true;
		} 
	}
	Super::DoJump(bReplayingMoves);
	return true;

}


