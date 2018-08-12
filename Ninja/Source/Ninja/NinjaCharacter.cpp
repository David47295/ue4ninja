// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaCharacter.h"
#include "NinjaMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"

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
	AttackHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitbox"));
	AttackHitbox->RelativeLocation = AttackHitboxLocation;
	AttackHitbox->SetActive(false);
	AttackHitbox->bHiddenInGame = false;
	AttackHitbox->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//CameraBoom->SetWorldRotation(FRotator(0.f, -180.f, 0.f));
	//CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	MainCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	Sprite->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
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
	SetAttackHitboxLocation();

	HandleAttack();
	
}

// Called to bind functionality to input
void ANinjaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANinjaCharacter::MoveRight(float Value) {
	AddMovementInput(GetActorRightVector(), Value);
}



void ANinjaCharacter::Attack() {
	bIsAttacking = true;
	AttackHitbox->SetActive(true);
	if (!(GetCharacterMovement()->IsFalling())) {
		APlayerController* MyController = (APlayerController*)GetController();
		if (MyController) { DisableInput(MyController); }
	}
}

void ANinjaCharacter::SetAttackHitboxLocation()
{
	float right = GetInputAxisValue("MoveRight");
	if (right != 0 && !bIsAttacking) {
		// If player is looking left
		if (right > 0) {
			FVector dir = AttackHitboxLocation;
			dir.Y *= -1.f;
			AttackHitbox->SetRelativeLocation(dir);
			
		}
		// if player is looking right
		else {
			AttackHitbox->SetRelativeLocation(AttackHitboxLocation);
		}
	}

}

void ANinjaCharacter::HandleAttack()
{
	if (bIsAttacking) {
		int32 AnimLength = Sprite->GetFlipbookLengthInFrames();
		int32 CurrFrame = Sprite->GetPlaybackPositionInFrames();
		if (GrndAttackStartFrame <= CurrFrame && CurrFrame <= GrndAttackEndFrame) {
			//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, FString::Printf(TEXT("Frame: %d"), CurrFrame));
			TArray<AActor*> Actors = TArray<AActor*>();
			AttackHitbox->GetOverlappingActors(Actors);
			ANinjaCharacter* Target = (ANinjaCharacter*)Actors.GetData();
			if (Target) {
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("HIT Frame: %d"), CurrFrame));

			}
		}
		else if (CurrFrame >= AnimLength - 1) {
			GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("Enabling input"));
			APlayerController* MyController = (APlayerController*)GetController();
			if (MyController) {
				EnableInput(MyController);
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("Error"));
			}
			AttackHitbox->SetActive(false);
			bIsAttacking = false;
		}
	}
}

