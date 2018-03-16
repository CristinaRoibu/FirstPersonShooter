// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "PewPewCharacter.h"
#include "PewPewProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Gun.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "MeinCharacter.h"



DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// APewPewCharacter

APewPewCharacter::APewPewCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	TheMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TheMesh"));
	TheMesh->SetOnlyOwnerSee(true);
	TheMesh->SetupAttachment(FirstPersonCameraComponent);
	TheMesh->bCastDynamicShadow = false;
	TheMesh->CastShadow = false;
	TheMesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	TheMesh->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->Hand = EControllerHand::Right;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.


	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void APewPewCharacter::BeginPlay()
{

	// Call the base class  
	Super::BeginPlay();

	if (!GunBlueprint) {
		UE_LOG(LogTemp, Warning, TEXT("GUN BP MISSING"))return; }
	 Gun = GetWorld()->SpawnActor<AGun>( GunBlueprint);
	 Gun->AttachToComponent(TheMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint_2"));
	 Gun->bIsHitscan = true;

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APewPewCharacter::TouchStarted);
	InputComponent->BindAction("Fire", IE_Pressed, Gun, &AGun::OnFire);
}

//////////////////////////////////////////////////////////////////////////
// Input

void APewPewCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	/*if (EnableTouchscreenMovement(PlayerInputComponent) == false)
	{
		PlayerInputComponent->BindAction("Fire", IE_Pressed, Gun, &AGun::OnFire);
	}*/

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APewPewCharacter::OnResetVR);

	PlayerInputComponent->BindAction("ShootinLine", IE_Pressed, this, &APewPewCharacter::DrawLine);

	PlayerInputComponent->BindAxis("MoveForward", this, &APewPewCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APewPewCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APewPewCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APewPewCharacter::LookUpAtRate);
}

void APewPewCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void APewPewCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void APewPewCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		//OnFire();
	}
	TouchItem.bIsPressed = false;
}

void APewPewCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APewPewCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APewPewCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APewPewCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool APewPewCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APewPewCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &APewPewCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APewPewCharacter::TouchUpdate);
	}
	return bResult;
}

void APewPewCharacter::DrawLine() {
	
	FHitResult OutHit;
	FVector CameraLocation =  (FirstPersonCameraComponent->GetComponentLocation()); //FirstPersonCameraComponent->GetForwardVector() +
	//LineTraceSingleByChannel(struct FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params /* = FCollisionQueryParams::DefaultQueryParam */, const FCollisionResponseParams& ResponseParam /* = FCollisionResponseParams::DefaultResponseParam */) const
	GetWorld()->LineTraceSingleByChannel(OutHit, CameraLocation + (FirstPersonCameraComponent->GetForwardVector() * 50), (CameraLocation + (FirstPersonCameraComponent->GetForwardVector() * 2000)), ECollisionChannel::ECC_Visibility);
	UGameplayStatics::PlaySoundAtLocation(this, ShootinSound, GetActorLocation());

	PlayFireAnimation();

	AActor* HitActor = OutHit.GetActor();
	AMeinCharacter* VarOfMeinChar = Cast<AMeinCharacter>(HitActor);
	if (VarOfMeinChar) { VarOfMeinChar->TakeDamage(); }
	
	//OutHit.TakeDamage();
	//UObject* WorldContextObject, FVector const LineStart, FVector const LineEnd, FLinearColor Color, float LifeTime, float Thickness)
	//if (ForwardOfCamera.Length() == 0.0f)return;
	//UKismetSystemLibrary::DrawDebugLine(this, CameraLocation + (FirstPersonCameraComponent->GetForwardVector() * 50), (CameraLocation + (FirstPersonCameraComponent->GetForwardVector() * 2000)), FLinearColor::Red, 50.f, 3.0);
	if (HitActor == nullptr )return;
	UE_LOG(LogTemp, Warning, TEXT("ForwardOfCamera, %s, %s"), *OutHit.GetActor()->GetName(), *OutHit.GetActor()->GetActorLocation().ToString() );
}

void APewPewCharacter::PlayFireAnimation_Implementation() {

}