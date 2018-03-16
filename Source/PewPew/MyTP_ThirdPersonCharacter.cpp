// Fill out your copyright notice in the Description page of Project Settings.

#include "MyTP_ThirdPersonCharacter.h"

void AMyTP_ThirdPersonCharacter::BeginPlay() {
	Super::BeginPlay();

	
}

TArray<AActor*> AMyTP_ThirdPersonCharacter::GetPatrolPoints() {
	return PatrolPointsu;
}
