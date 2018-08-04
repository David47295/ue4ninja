// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaCharacter.h"
#include "NinjaMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#define CAMERA_ARM_LENGTH 500.f

// Sets default values
ANinjaCharacter::ANinjaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UNinjaMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CameraBoom->TargetArmLength = CAMERA_ARM_LENGTH;
	//CameraBoom->SetWorldRotation(FRotator(0.f, -180.f, 0.f));
	//CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	MainCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

}

// Called when the game starts or when spawned
void ANinjaCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANinjaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANinjaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANinjaCharacter::MoveRight(float Value) {
	AddMovementInput(GetActorRightVector(), Value);
}