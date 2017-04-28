// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CatchMe.h"
#include "CatchMeCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "TableUtil.h"

ACatchMeCharacter::ACatchMeCharacter()
{
	LuaCtor("character.catchmecharacter", this);
}

void ACatchMeCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
