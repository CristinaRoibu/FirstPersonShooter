// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/Character.h"
#include "MeinCharacter.generated.h"

UCLASS()
class PEWPEW_API AMeinCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMeinCharacter();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, category = "Inherited")
	class UCameraComponent* CameraBoomBoom;

	UPROPERTY(VisibleDefaultsOnly, category = "Inherited")
	class USkeletalMeshComponent* Dummy;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void TakeDamage();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	int LifeSpan = 8;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	bool Attacked = false;

	UFUNCTION(BlueprintImplementableEvent,  Category = "Play Sound")
	void PlaySound();
	
};
