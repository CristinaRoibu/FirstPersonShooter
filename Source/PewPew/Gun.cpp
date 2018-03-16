// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "PewPewProjectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "PewPew.h"

// Sets default values
AGun::AGun()
{

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a gun mesh component
	TheGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TheGun"));
	//TheGun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	TheGun->bCastDynamicShadow = false;
	TheGun->CastShadow = false;
	//TheGun->SetupAttachment(TheMesh, TEXT("GripPoint_2"));
	//FP_Gun->SetupAttachment(RootComponent);


	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(TheGun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::OnFire()
{
	if (!bIsHitscan)
	{
		// try and fire a projectile
		if (ProjectileClass != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Theshooting if"))
				UWorld* const World = GetWorld();
			if (World != NULL)
			{
				if (bUsingMotionControllers)
				{
					//const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
					//const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
					//World->SpawnActor<APewPewProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
				}
				else
				{

					const FRotator SpawnRotation = FP_MuzzleLocation->GetComponentRotation(); // GetControlRotation(); 
					//MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					const FVector SpawnLocation = FP_MuzzleLocation->GetComponentLocation();

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;//spawn always even if colliding

					// spawn the projectile at the muzzle
					APewPewProjectile* Projectil = World->SpawnActor<APewPewProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

					//UKismetSystemLibrary::DrawDebugLine(this, SpawnLocation, (UKismetMathLibrary::GetForwardVector(SpawnRotation) * 500) + SpawnLocation, FLinearColor::Blue, 120.f, 5);
				}
			}
		}

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		//UAnimInstance* AnimInstance = TheMesh->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}