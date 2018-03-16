// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/Actor.h"
#include "Tile.generated.h"



USTRUCT(BlueprintType)
struct FSpawnPosition
{
	GENERATED_USTRUCT_BODY()

	FVector Location;
	float Rotation;
	float Size;
};


class UActorPool;

UCLASS()
class PEWPEW_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	UFUNCTION(BlueprintCallable, category = "Spawn")
	void PlaceActors(TSubclassOf<AActor> TheSpawnee);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<class AStaticMeshActor> TheSpawnee;

	UFUNCTION(BlueprintCallable, category = "Spawn")
	bool FindSuitableSpawnLocation(float Radius, FVector& OutVector);


	template<class T>
	void PlaceTheActors(TSubclassOf<T> TheSpawnee);

	UFUNCTION(BlueprintCallable, category = "Spawn")
	void SetActorPool(UActorPool* ToSet);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	FVector NavigationOffset;

	UFUNCTION(BlueprintCallable, category = "Spawn")
	TArray<FVector> SpawnPositions();
	UFUNCTION(BlueprintCallable, category = "Spawn")
	void PlaceAI(TSubclassOf<class APawn> ThePawn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason)override;
	 UActorPool* ActorPoolVar;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	TArray<class AActor*> PropsToDelete;

private:
	AActor* NavMeshBoundsVolume;
	
};
