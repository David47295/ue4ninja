// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaPlayerState.h"
#include "Net/UnrealNetwork.h"

void ANinjaPlayerState::SetChosenCharacter(TSubclassOf<ACharacter> Character)
{
	ChosenCharacter = Character;
	if (Role < ROLE_Authority) {
		Server_SetChosenCharacter(Character);
	}
}

void ANinjaPlayerState::Server_SetChosenCharacter_Implementation(TSubclassOf<ACharacter> Character)
{
	SetChosenCharacter(Character);
}

bool ANinjaPlayerState::Server_SetChosenCharacter_Validate(TSubclassOf<ACharacter> Character)
{
	return true;
}

TSubclassOf<ACharacter> ANinjaPlayerState::GetChosenCharacter() const {
	return ChosenCharacter;
}

