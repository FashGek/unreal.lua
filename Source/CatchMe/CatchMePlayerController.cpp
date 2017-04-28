// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CatchMe.h"
#include "CatchMePlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "CatchMeCharacter.h"
#include "TableUtil.h"

ACatchMePlayerController::ACatchMePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	LuaCtor("controller.cmplayercontroller", this);
}

void ACatchMePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}



void ACatchMePlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	LuaCall("MoveToPos", this, DestLocation);
}


void ACatchMePlayerController::MoveToLocation_Implementation(FVector Location)
{
	SetNewMoveDestination(Location);
}

bool ACatchMePlayerController::MoveToLocation_Validate(FVector Location)
{
	return true;
}