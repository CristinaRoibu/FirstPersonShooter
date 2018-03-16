// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "PewPew.h"
#include "Engine/StaticMeshActor.h"
#include "DrawDebugHelpers.h"
#include "ActorPool.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	NavigationOffset = FVector(2000,0,0); //TODO add it to the new location of the NavMesh
}



void ATile::PlaceActors(TSubclassOf<AActor> TheSpawnee) {
	
	TArray<FVector> SpawnPositionsS = SpawnPositions();
	for (int i = 1; i < SpawnPositionsS.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("TRULU_Pawns"))
			AStaticMeshActor* Spawned = GetWorld()->SpawnActor<AStaticMeshActor>(TheSpawnee); //, RandomPoint, FRotator(0.f, 0.f ,0.f)
		float RandomYaw = FMath::RandRange(180.f, -180.f);
		//Spawned->SetActorRelativeLocation(RandomPoint); //wo it will spawn in the specific tile...not in the world loc
		Spawned->SetActorLocation(SpawnPositionsS[i] + FVector(0.f, 0.f, 0)); //wo it will spawn in the specific tile...not in the world loc
		Spawned->SetActorRotation(FRotator(GetActorRotation().Roll, RandomYaw, GetActorRotation().Pitch));
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false)); // prende la coordinata world del cuscino, gli aggiunge l'ActorLocation della sua tile rendendolo in relative space della sua tile e lo sposta li
				
		PropsToDelete.Add(Spawned);
	}

}

TArray<FVector> ATile::SpawnPositions()
{
	TArray<FVector> SpawnPositions;
	FVector OutVector;
	for (size_t i = 0; i < 3; i++)
	{
		while (FindSuitableSpawnLocation(140.f, OutVector)) {}//will do the while uintil it finds a legit point to spawn a prop
		SpawnPositions.Push(OutVector);
	}
	return SpawnPositions;
}

void ATile::PlaceAI(TSubclassOf<class APawn> ThePawn)
{
	TArray<FVector> SpawnPositionsS = SpawnPositions();
	for (int i = 1; i < SpawnPositionsS.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("TRULU_Pawns"))
		APawn* Spawned = GetWorld()->SpawnActor<APawn>(ThePawn); //, RandomPoint, FRotator(0.f, 0.f ,0.f)
		float RandomYaw = FMath::RandRange(180.f, -180.f);
		//Spawned->SetActorRelativeLocation(RandomPoint); //wo it will spawn in the specific tile...not in the world loc
		Spawned->SetActorLocation(SpawnPositionsS[i] + FVector(0.f, 0.f, 133.f)); //wo it will spawn in the specific tile...not in the world loc
		Spawned->SetActorRotation(FRotator(GetActorRotation().Roll, RandomYaw, GetActorRotation().Pitch));
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false)); // prende la coordinata world del cuscino, gli aggiunge l'ActorLocation della sua tile rendendolo in relative space della sua tile e lo sposta li
		PropsToDelete.Add(Spawned);																						//Spawned->AttachRootComponentToActor(this);	
	}

}

template<class T>
void ATile::PlaceTheActors(TSubclassOf<T> TheSpawnee)
{
	TArray<FVector> SpawnPositionsS = SpawnPositions();
	for (int i = 1; i < SpawnPositionsS.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("TRULU_Pawns"))
		T* Spawned = GetWorld()->SpawnActor<T>(TheSpawnee); //, RandomPoint, FRotator(0.f, 0.f ,0.f)
		float RandomYaw = FMath::RandRange(180.f, -180.f);
		//Spawned->SetActorRelativeLocation(RandomPoint); //wo it will spawn in the specific tile...not in the world loc
		Spawned->SetActorLocation(SpawnPositionsS[i] + FVector(0.f, 0.f, 133.f)); //wo it will spawn in the specific tile...not in the world loc
		Spawned->SetActorRotation(FRotator(GetActorRotation().Roll, RandomYaw, GetActorRotation().Pitch));
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false)); // prende la coordinata world del cuscino, gli aggiunge l'ActorLocation della sua tile rendendolo in relative space della sua tile e lo sposta li
																									//Spawned->AttachRootComponentToActor(this);	
	}
}

bool ATile::FindSuitableSpawnLocation(float Radius, FVector& OutVector)
{
	
	FVector RandomPoint = FMath::RandPointInBox(FBox(FVector(0, -2000, -173) + GetActorLocation(), FVector(4000, 2000, -173) + GetActorLocation()));
	FHitResult OutHit;
	//location is for start and end bc the sphere will not move in the world
	bool HitSomething = GetWorld()->SweepSingleByChannel(OutHit, RandomPoint, RandomPoint, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, FCollisionShape::MakeSphere(Radius));
	if (!HitSomething)
	{
		OutVector = RandomPoint;
	}
	return HitSomething;
	/*
	FColor Color;
	if (HitSomething) { Color = FColor::Red; }else { Color = FColor::Green; }
	DrawDebugSphere(GetWorld(), RandomPoint, Radius, 30, Color, true, 100); //const UWorld* InWorld, FVector const& Center, float Radius, int32 Segments, FColor const& Color, bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0, float Thickness = 0.f) {}
	*/
	
}


// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ActorPoolVar->ReturnNav(NavMeshBoundsVolume);//return the nav mesh in the pool
}


void ATile::SetActorPool(UActorPool* ToSet)
{
	UE_LOG(LogTemp, Warning, TEXT("Found volume: %s"), *ToSet->GetName())
		ActorPoolVar = ToSet;
	NavMeshBoundsVolume = ActorPoolVar->GiveNav(); //set this actorPoolVar the location of this tile
	if (NavMeshBoundsVolume == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Nullptr Pool empty"))
			return;
	}
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationOffset);
	GetWorld()->GetNavigationSystem()->Build();
}
