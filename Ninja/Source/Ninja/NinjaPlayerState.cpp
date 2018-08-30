// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaPlayerState.h"
#include "Net/UnrealNetwork.h"

void ANinjaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANinjaPlayerState, bIsMyTurn);
	
	
}