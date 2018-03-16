// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTChoosePatrol.h"
#include "PewPew.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "MyTP_ThirdPersonCharacter.h"
#include "PatrolPoints.h"



EBTNodeResult::Type UBTTChoosePatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	
	UBlackboardComponent* BbComp = OwnerComp.GetBlackboardComponent();
	if (!BbComp) { return EBTNodeResult::Failed; }
	auto Index = BbComp->GetValueAsInt(IndexKey.SelectedKeyName);//ask to what the IndexKey points to in the BB


	//Set next waypoint
	SetPawn(OwnerComp);
	if (!PatrolComponent) { return EBTNodeResult::Failed; }
	PatrolPoints = PatrolComponent->GetPatrolPoints();
	if (PatrolPoints.Num() == 0) { return EBTNodeResult::Failed; }
	BbComp->SetValueAsObject(WaypointKey.SelectedKeyName, PatrolPoints[Index]);	// (OwnerComp, Index, Index);  UBTNode* NodeOwner, const FBlackboardKeySelector& Key, UObject* Value
	
	//Cycle the index
	auto NextIndex = (Index + 1) % PatrolPoints.Num();
	BbComp->SetValueAsInt(IndexKey.SelectedKeyName, NextIndex);

	return EBTNodeResult::Succeeded;
}

void UBTTChoosePatrol::SetPawn(UBehaviorTreeComponent& OwnerComp) {
	PatrolComponent = Cast<UPatrolPoints>(OwnerComp.GetAIOwner()->GetPawn()->GetComponentByClass(UPatrolPoints::StaticClass()));
}
