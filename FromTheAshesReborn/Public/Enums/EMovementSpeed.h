// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) enum class EMovementSpeed : uint8
{

	EMovementSpeed_Idle			UMETA(DisplayName = "Idle"),
	EMovementSpeed_Walking		UMETA(DisplayName = "Walking"),
	EMovementSpeed_Jogging		UMETA(DisplayName = "Jogging"),
	EMovementSpeed_Sprinting	UMETA(DisplayName = "Sprinting"),

};

