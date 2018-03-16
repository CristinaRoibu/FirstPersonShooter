// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Components/ActorComponent.h"
#include "PatrolPoints.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PEWPEW_API UPatrolPoints : public UActorComponent
{
	GENERATED_BODY()


protected:

	UPROPERTY(EditAnywhere)
	TArray<AActor*> PatrolPoints;

public:	

	TArray<AActor*> GetPatrolPoints();
	
};
