// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UnrealNetwork.h"
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

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const override;

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

	/**
	Set bIsMoving. Sets to true if Value != 0 and false otherwise
	@param Value the MoveRight axis value of the controller
	*/
	UFUNCTION(BlueprintCallable, Category = "Ninja Movement")
		virtual void SetIsMoving(float Value);
	
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Ninja Movement")
		virtual void ServerSetIsMoving(float Value);

	virtual void SetSpriteRotation();

	UFUNCTION(Server, unreliable, WithValidation, Category="Ninja Movement")
		virtual void ServerSetSpriteRotation(float Value);

	virtual void SetAttackHitboxLocation(float Value);

	UFUNCTION(Server, unreliable, WithValidation, Category = "Ninja Attacking")
		virtual void ServerSetAttackHitboxLocation(float Value);

	UFUNCTION()
		//virtual void HandleAttack(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		virtual void HandleAttack();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ninja Attacking")
		float AttackAnimLength;

	UPROPERTY()
		float AttackAnimTimer;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Ninja Movement")
		bool bIsMoving;
	//virtual bool AttackConnected();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ninja Attacking")
		int32 GrndAttackStartFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ninja Attacking")
		int32 GrndAttackEndFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ninja Movement")
		FRotator SpriteRightRot = FRotator(0.f, 90.f, 0.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ninja Movement")
		FRotator SpriteLeftRot = FRotator(0.f, -90.f, 0.f);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
