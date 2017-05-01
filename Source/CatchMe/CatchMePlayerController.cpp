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

void ACatchMePlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	auto result = LuaStaticCallr(TArray<FReplifetimeCond>, "CMPlayerController_GetLifetimeReplicatedProps", this);
	for (auto &v : result)
	{
		UProperty* p = UTableUtil::GetPropertyByName(ACatchMePlayerController::StaticClass(), v.PropertyName);
		for (int32 i = 0; i < p->ArrayDim; i++)
		{
			OutLifetimeProps.AddUnique(FLifetimeProperty(p->RepIndex + i, v.Cond));
		}
	}
}

void ACatchMePlayerController::S_MoveToLocation_Implementation(FVector Location)
{
	LuaCall("MoveToPos", this, Location);
}

bool ACatchMePlayerController::S_MoveToLocation_Validate(FVector Location)
{
	return true;
}