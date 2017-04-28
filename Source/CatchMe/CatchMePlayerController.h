// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "controller/CMPlayerControllerBase.h"
#include "CatchMePlayerController.generated.h"

UCLASS(meta=(lua=1), minimalapi)
class ACatchMePlayerController : public ACMPlayerControllerBase
{
	GENERATED_BODY()

public:
	ACatchMePlayerController();
	
	virtual void PlayerTick(float DeltaTime) override;

	void SetNewMoveDestination(const FVector DestLocation);

	UFUNCTION(reliable, server, WithValidation)
	void MoveToLocation(FVector Location);
};


