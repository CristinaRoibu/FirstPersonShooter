// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "MyTP_ThirdPersonCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PEWPEW_API AMyTP_ThirdPersonCharacter : public ATP_ThirdPersonCharacter
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<AActor*> PatrolPointsu;
	
	TArray<AActor*> GetPatrolPoints();


};
