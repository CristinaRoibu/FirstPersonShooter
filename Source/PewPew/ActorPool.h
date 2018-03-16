// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Components/ActorComponent.h"
#include "ActorPool.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PEWPEW_API UActorPool : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorPool();

	AActor* GiveNav();//return a pointer to an actor(the navMesh)
	void ReturnNav(AActor* ActorToReturn);//Nav mesh is returned after being used

	void AddNav(AActor* ActorToAdd);

protected:

private:
	TArray<AActor*> Pool;
	
	
};
