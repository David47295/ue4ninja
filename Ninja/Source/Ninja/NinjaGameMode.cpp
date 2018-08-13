// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaGameMode.h"
#include "NinjaCharacter.h"


ANinjaGameMode::ANinjaGameMode() {
	DefaultPawnClass = ANinjaCharacter::StaticClass();
}
