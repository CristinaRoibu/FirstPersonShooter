// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorPool.h"


// Sets default values for this component's properties
UActorPool::UActorPool()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

AActor * UActorPool::GiveNav()//returns the nav mesh
{
	UE_LOG(LogTemp, Warning, TEXT("Actor GiveNav: %s"), *GetName())
	if (Pool.Num() == 0) { return nullptr; }
	return Pool.Pop();//toglir l elemento dall array
}

void UActorPool::ReturnNav(AActor * ActorToReturn)
{
	if (ActorToReturn == nullptr) { return; }
	Pool.Push(ActorToReturn);
	UE_LOG(LogTemp, Warning, TEXT("Actor returned: %s"), *ActorToReturn->GetName())
}

void UActorPool::AddNav(AActor * ActorToAdd)
{
	if (ActorToAdd == nullptr) { return; }
	Pool.Push(ActorToAdd);
	UE_LOG(LogTemp, Warning, TEXT("Actor AddNav: %s"), *ActorToAdd->GetName())
}
