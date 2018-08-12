// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NinjaCharacter.generated.h"

// Forward declarations
class UCameraComponent;
class USpringArmComponent;
class UBoxComponent;
class UPaperFlipbookComponent;

UCLASS()
class NINJA_API ANinjaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANinjaCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "NinjaMovement")
		virtual void MoveRight(float Value);

	UCameraComponent* MainCamera;

	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ninja Attacking")
		UBoxComponent* AttackHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ninja Attacking")
		FVector AttackHitboxLocation = FVector(0.f, -80.f, 30.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ninja Sprite")
		UPaperFlipbookComponent* Sprite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ninja Attacking")
		bool bIsAttacking;

	UFUNCTION(BlueprintCallable, Category="Ninja Attacking")
		virtual void Attack();

	virtual void SetAttackHitboxLocation();

	virtual void HandleAttack();

	//virtual bool AttackConnected();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ninja Attacking")
		int32 GrndAttackStartFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ninja Attacking")
		int32 GrndAttackEndFrame;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
