// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void ANinjaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANinjaPlayerState, bIsMyTurn);
	
	
}

void ANinjaPlayerState::OnRep_IsMyTurn_Implementation()
{
	APlayerController* PlayerCont = (APlayerController*)GetOwner();
	GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("Running Replication function"));
	if (PlayerCont) {
		if (!bIsMyTurn) {
			PlayerCont->DisableInput(PlayerCont);
		}
		else {
			PlayerCont->EnableInput(PlayerCont);
		}
	}
}

bool ANinjaPlayerState::OnRep_IsMyTurn_Validate()
{
	return true;
}

bool ANinjaPlayerState::GetIsMyTurn() const
{
	return bIsMyTurn;
}

void ANinjaPlayerState::SetIsMyTurn(bool turn)
{
	bIsMyTurn = turn;
}
