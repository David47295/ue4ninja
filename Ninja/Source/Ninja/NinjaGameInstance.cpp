// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaGameInstance.h"

int32 UNinjaGameInstance::GetNumRounds() const
{
	return NumRounds;
}

void UNinjaGameInstance::SetNumRounds(int32 Num) {
	NumRounds = Num;
}