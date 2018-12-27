// Fill out your copyright notice in the Description page of Project Settings.

#include "NinjaCharacter.h"
#include "NinjaMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "NinjaPlayerState.h"
#include "NinjaGameModeBase.h"
#include "NinjaGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"



#define CAMERA_ARM_LENGTH 500.f
#define WORLD_FREEZE_TIME_SCALE 1.f
#define WORLD_REAL_TIME_SCALE 1.f

// Sets default values
ANinjaCharacter::ANinjaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UNinjaMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = CAMERA_ARM_LENGTH;
	AttackHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitbox"));
	AttackHitbox->RelativeLocation = AttackHitboxLocation;
	AttackHitbox->bHiddenInGame = false;
	AttackHitbox->SetupAttachment(RootComponent);
	MainCamera->SetupAttachment(CameraBoom);
	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(RootComponent);
	Sprite->SetIsReplicated(true);
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
	SetWorldTime(WORLD_FREEZE_TIME_SCALE);
	Sprite->SetFlipbook(IdleAnimFlipbook);
}

bool ANinjaCharacter::IsFlipbookPlaying(UPaperFlipbook * Flipbook) const
{
	return Sprite->GetFlipbook() == Flipbook;
}

// Called every frame
void ANinjaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//SetAttackHitboxLocation(right);
	HandleAttack();
	//HandleAnimations();


}

// Called to bind functionality to input
void ANinjaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANinjaCharacter::MoveRight);
}

void ANinjaCharacter::MoveRight(float AxisValue) {
	AddMovementInput(GetActorRightVector(), AxisValue);
	HandleAnimations();
}


void ANinjaCharacter::HandleAnimations()
{
	float right = GetInputAxisValue("MoveRight");
	Server_HandleAnimations(right);
}

void ANinjaCharacter::Server_HandleAnimations_Implementation(float Dir)
{
	
	UCharacterMovementComponent* CharMov = GetCharacterMovement();
	if (CharMov) {
		ServerSetSpriteRotation(Dir);
		if (!CharMov->IsFalling() && !bIsAttacking) {
			if (Dir != 0.f && !IsFlipbookPlaying(WalkAnimFlipbook)) {
				GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("Walking")));
				Sprite->SetFlipbook(WalkAnimFlipbook);
			} else if (Dir == 0.f) {
				Sprite->SetFlipbook(IdleAnimFlipbook);
			}
		}
		 
	}
}

bool ANinjaCharacter::Server_HandleAnimations_Validate(float Dir)
{
	return true;
}


void ANinjaCharacter::Attack_Implementation() {
	UWorld* World = GetWorld();
	if (World) {
		bIsAttacking = true;
		AttackAnimLength = AttackAnimFlipbook->GetTotalDuration();
		ANinjaGameModeBase* GameMode = (ANinjaGameModeBase*)World->GetAuthGameMode();
		if (GameMode) {

			UNinjaMovementComponent* CharMov = (UNinjaMovementComponent*)GetCharacterMovement();
			if (CharMov) {
				CharMov->Dash();
			}

			World->GetTimerManager().SetTimer(GameMode->ActionPhaseTimerHandle, this, &ANinjaCharacter::FreezeTime , AttackAnimLength, false);
		}
	}
}

bool ANinjaCharacter::Attack_Validate() {
	return true;
}

void ANinjaCharacter::SetWorldTime_Server_Implementation(float scale)
{
	SetWorldTime(scale);
	SetWorldTime_Client(scale);
}

bool ANinjaCharacter::SetWorldTime_Server_Validate(float scale)
{
	return true;
}

void ANinjaCharacter::SetWorldTime_Client_Implementation(float scale) {
	SetWorldTime(scale);
}

void ANinjaCharacter::SetWorldTime(float scale)
{
	UWorld* World = GetWorld();
	if (World) {
		for (TActorIterator<ANinjaCharacter> ActorItr(World); ActorItr; ++ActorItr) {
			ANinjaCharacter* Char = *ActorItr;
			Char->CustomTimeDilation = scale;
		}
	}
}

void ANinjaCharacter::FreezeTime()
{
	UNinjaMovementComponent* CharMov = (UNinjaMovementComponent*)GetCharacterMovement();
	if (CharMov) {
		CharMov->StopDash();
		AttackHitbox->SetActive(false);
		bIsAttacking = false;
	}

	if (Role < ROLE_Authority) {
		SetWorldTime_Server(WORLD_FREEZE_TIME_SCALE);
	}
	else {
		SetWorldTime_Client(WORLD_FREEZE_TIME_SCALE);
	}
	
	SetWorldTime(WORLD_FREEZE_TIME_SCALE);
	
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
			TArray<AActor*> Actors = TArray<AActor*>();
			AttackHitbox->GetOverlappingActors(Actors, ACharacter::StaticClass());
			ANinjaCharacter* Target = (ANinjaCharacter*)Actors.GetData();
			if (Target) {
				Server_HandleAttack();
			}
		}

	}
		
}

void ANinjaCharacter::Server_HandleAttack_Implementation()
{
	if (bIsAttacking) {
		TArray<AActor*> Actors = TArray<AActor*>();
		AttackHitbox->GetOverlappingActors(Actors, ACharacter::StaticClass());
		ANinjaCharacter* Target = (ANinjaCharacter*)Actors.GetData();
		if (Target) {
			//AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			RegisterHit();
		}
	}
}

bool ANinjaCharacter::Server_HandleAttack_Validate()
{
	return true;
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
				//GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Blue, FString::Printf(TEXT("Score: %f"), PS->Score));
				ANinjaGameModeBase* GameMode = (ANinjaGameModeBase*)World->GetAuthGameMode();
				if (GameMode) {
					int32 Round = GameMode->GetCurrentRound();
					if (Round < GameMode->RoundLimit - 1) {
						GameMode->SetCurrentRound(Round + 1);
						GameMode->BeginRound();
					}
					else {
						ANinjaGameStateBase* GameState = (ANinjaGameStateBase*)GameMode->GetGameState<ANinjaGameStateBase>();
						if (GameState) {
							ShowEndScreen();
						}
					}
				}
			}
		}
	}
}

bool ANinjaCharacter::RegisterHit_Validate()
{
	return true;
}


void ANinjaCharacter::ShowEndScreen_Implementation()
{
	UWorld* World = GetWorld();
	if (World) {
		ANinjaGameStateBase* GameState = (ANinjaGameStateBase*)World->GetGameState();
		APlayerController* Cont = UGameplayStatics::GetPlayerController(World, 0);
		if (GameState) {
			if (Cont) {
				UUserWidget* Screen = CreateWidget<UUserWidget>(Cont, GameState->EndScreenWidget);
				if (Screen) {
					FInputModeUIOnly Mode;
					Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
					Cont->SetInputMode(Mode);
					Cont->bShowMouseCursor = true;
					Screen->AddToViewport(99);
				}
			}
		}
	}

	
}