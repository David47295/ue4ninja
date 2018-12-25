// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NinjaGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NINJA_API UNinjaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ninja Instance")
		void SetNumRounds(int32 Num);

	UFUNCTION(BlueprintCallable, Category="Ninja Instance")
		int32 GetNumRounds() const;
	
private:
	int32 NumRounds;
	
	
};
