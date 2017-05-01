// Fill out your copyright notice in the Description page of Project Settings.

#include "CatchMe.h"
#include "CatchMeAIController.h"
#include "tableutil.h"



ACatchMeAIController::ACatchMeAIController()
{
	LuaCtor("controller.catchmeaicontroller", this);
}
