// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once 


#include "GameFramework/HUD.h"
#include "PewPewHUD.generated.h"

UCLASS()
class APewPewHUD : public AHUD
{
	GENERATED_BODY()

public:
	APewPewHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

