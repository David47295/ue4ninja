// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaGameModeBase.h"
#include "NinjaCharacter.h"

ANinjaGameModeBase::ANinjaGameModeBase() {
	DefaultPawnClass = ANinjaCharacter::StaticClass();
}

void ANinjaGameModeBase::StartRound() {

}


