// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "BehaviorTree/BTTaskNode.h"
#include "BTTChoosePatrol.generated.h"


/**
 * 
 */
UCLASS()
class PEWPEW_API UBTTChoosePatrol : public UBTTaskNode
{
	GENERATED_BODY()


		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector IndexKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector WaypointKey;

	void SetPawn(UBehaviorTreeComponent& OwnerComp);

	class UPatrolPoints* PatrolComponent;

	TArray<AActor*> PatrolPoints;
};
