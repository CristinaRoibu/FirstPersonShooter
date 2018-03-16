// Fill out your copyright notice in the Description page of Project Settings.

#include "TheInfiniteTerrainGameMode.h"
#include "AI/Navigation/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "ActorPool.h"

ATheInfiniteTerrainGameMode::ATheInfiniteTerrainGameMode()
{
	//UActorPool* VarNavMesh = Cast<UActorPool>(CreateDefaultSubobject<AActor>(TEXT("Pool")));
	ActorPoolVar = CreateDefaultSubobject<UActorPool>(TEXT("ActroPoolNavMesh"));
	
	
}

void ATheInfiniteTerrainGameMode::BeginPlay() {
	Super::BeginPlay();
	//PopulateBoundsVolumePool();
}



//method called once and will collect all tne nav meshes is the world and put them in the array
void ATheInfiniteTerrainGameMode::PopulateBoundsVolumePool() {
	
	TActorIterator<ANavMeshBoundsVolume> NavMeshIterator = TActorIterator<ANavMeshBoundsVolume>(GetWorld()); //AActorIterator can be used as a boolean. if its true its bc there is still an actor to iterate
	while (NavMeshIterator) {//AActorIterator has a list and you get hold of a element at a time
		addToPool(*NavMeshIterator);  //AActor* FoundActor = *NavMeshIterator; //the content of AActorIterator is a Actor pointer and you access it withe the star *
		++NavMeshIterator;
	}
	
}

void ATheInfiniteTerrainGameMode::addToPool(ANavMeshBoundsVolume* TheNavMesh) {
	UE_LOG(LogTemp, Warning, TEXT("Found volume: %s"), *TheNavMesh->GetName())
	ActorPoolVar->AddNav(TheNavMesh);
}


