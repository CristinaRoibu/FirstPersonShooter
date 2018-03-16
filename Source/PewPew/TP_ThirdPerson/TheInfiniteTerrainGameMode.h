// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PewPewGameMode.h"
#include "TheInfiniteTerrainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PEWPEW_API ATheInfiniteTerrainGameMode : public APewPewGameMode
{
	GENERATED_BODY()
public:
	ATheInfiniteTerrainGameMode();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void PopulateBoundsVolumePool();
	
	void addToPool(class ANavMeshBoundsVolume* TheNavMesh);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Setup")
	class UActorPool* ActorPoolVar;
	
};
