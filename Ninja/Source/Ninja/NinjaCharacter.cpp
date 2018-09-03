// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaCharacter.h"
#include "NinjaMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "NinjaPlayerState.h"
#include "NinjaGameModeBase.h"

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
	AttackHitbox->bHiddenInGame = false;
	AttackHitbox->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//CameraBoom->SetWorldRotation(FRotator(0.f, -180.f, 0.f));
	//CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	MainCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	Sprite->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Sprite->SetIsReplicated(true);

	//AttackHitbox->OnComponentBeginOverlap.AddDynamic(this, &ANinjaCharacter::HandleAttack);
}

void ANinjaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANinjaCharacter, bIsMoving);
}

// Called when the game starts or when spawned
void ANinjaCharacter::BeginPlay()
{
	Super::BeginPlay();
	//AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ANinjaCharacter::RegisterHit_Implementation()
{
	UWorld* World = GetWorld();
	if (World) {
		AController* Controller = GetController();
		if (Controller) {
			ANinjaPlayerState* PS = (ANinjaPlayerState*)Controller->PlayerState;
			if (PS) {
				PS->Score++;
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("Score: %f"), PS->Score));
				ANinjaGameModeBase* GameMode = (ANinjaGameModeBase*) World->GetAuthGameMode();
				if (GameMode) {
					GameMode->BeginRound();
				}
			}
		}
	}
}

bool ANinjaCharacter::RegisterHit_Validate()
{
	return true;
}


// Called every frame
void ANinjaCharacter::Tick(float DeltaTime)
{
	float right = GetInputAxisValue("MoveRight");
	Super::Tick(DeltaTime);
	SetSpriteRotation();
	SetAttackHitboxLocation(right);
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
	//AttackHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UNinjaMovementComponent* CharMov = (UNinjaMovementComponent*)GetCharacterMovement();
	if (CharMov) {
		if (!(CharMov->IsFalling())) {
			APlayerController* MyController = (APlayerController*)GetController();
			if (MyController) {
				DisableInput(MyController);
			}
		}
		CharMov->SetAttackDashDirection();
	}
	AttackAnimTimer = AttackAnimLength;
	
}

void ANinjaCharacter::SetIsMoving(float Value)
{
	APlayerController* PC = (APlayerController*)GetController();
	if (Role < ROLE_Authority && PC && PC->IsLocalController()) {
		//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, TEXT("Running Server code"));
		ServerSetIsMoving(Value);
	}

	bIsMoving = Value != 0.f;
}

void ANinjaCharacter::ServerSetIsMoving_Implementation(float Value)
{
	SetIsMoving(Value);
}

bool ANinjaCharacter::ServerSetIsMoving_Validate(float Value)
{
	return (-1.f <= Value && Value <= 1.f);
}

void ANinjaCharacter::SetSpriteRotation()
{
	APlayerController* PC = (APlayerController*)GetController();
	float right = GetInputAxisValue("MoveRight");

	if (Role < ROLE_Authority && PC && PC->IsLocalController()) {
		ServerSetSpriteRotation(right);
	}

	if (right > 0.f) {
		Sprite->SetRelativeRotation(SpriteRightRot);
	}
	else if (right < 0.f) {
		Sprite->SetRelativeRotation(SpriteLeftRot);
	}
}

void ANinjaCharacter::ServerSetSpriteRotation_Implementation(float Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("Value: %f"), Value));
	if (Value > 0.f) {
		Sprite->SetRelativeRotation(SpriteRightRot);
	}
	else if (Value < 0.f) {
		Sprite->SetRelativeRotation(SpriteLeftRot);
	}
}

bool ANinjaCharacter::ServerSetSpriteRotation_Validate(float Value)
{
	return (-1.f <= Value && Value <= 1.f);
}

void ANinjaCharacter::SetAttackHitboxLocation(float Value)
{
	APlayerController* PC = (APlayerController*)GetController();
	if (Role < ROLE_Authority && PC && PC->IsLocalController()) {
		ServerSetAttackHitboxLocation(Value);
	}

	if (Value != 0 && !bIsAttacking) {
		// If player is looking left
		if (Value > 0) {
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

void ANinjaCharacter::ServerSetAttackHitboxLocation_Implementation(float Value)
{
	SetAttackHitboxLocation(Value);
}

bool ANinjaCharacter::ServerSetAttackHitboxLocation_Validate (float Value)
{
	return (-1.f <= Value && Value <= 1.f);
}

void ANinjaCharacter::HandleAttack()
{
	if (bIsAttacking) {
		if (Sprite) {
			int32 AnimLength = Sprite->GetFlipbookLengthInFrames();
			AttackCurrFrame = Sprite->GetPlaybackPositionInFrames();
			if (GrndAttackStartFrame <= AttackCurrFrame && AttackCurrFrame <= GrndAttackEndFrame) {
				//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, FString::Printf(TEXT("Frame: %d"), CurrFrame));
				TArray<AActor*> Actors = TArray<AActor*>();
				AttackHitbox->GetOverlappingActors(Actors);
				ANinjaCharacter* Target = (ANinjaCharacter*)Actors.GetData();
				if (Target) {
					GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("HIT Frame: %d"), AttackCurrFrame));
					AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					RegisterHit();
				}
			}
			else if (AttackCurrFrame >= AnimLength - 1) {
				//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Red, TEXT("Enabling input"));
				APlayerController* MyController = (APlayerController*)GetController();
				if (MyController) {
					EnableInput(MyController);
				}
				AttackHitbox->SetActive(false);
				bIsAttacking = false;
			}
		}
	}
		
}

