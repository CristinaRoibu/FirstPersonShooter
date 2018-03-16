// Fill out your copyright notice in the Description page of Project Settings.

#include "MeinCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
AMeinCharacter::AMeinCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoomBoom = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraBoom"));
	CameraBoomBoom->SetupAttachment(RootComponent);
	//CameraBoomBoom->SetOnlyOwnerSee(true);

	Dummy = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Dummy"));
	Dummy->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMeinCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMeinCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMeinCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMeinCharacter::TakeDamage() {
	LifeSpan -= 1;
	Attacked = true;
	UE_LOG(LogTemp, Warning, TEXT("TakeDamage, %i"), LifeSpan);
	AController* Controller = GetController();
	PlaySound();
	//if (LifeSpan == 2) Attacked = false;
	if (LifeSpan <= 0 && Controller) {
		Controller->UnPossess();
	}
}

//void AMeinCharacter::PlaySound() {}