// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType) enum class EHitDirection : uint8
{
	EHitDirection_None		UMETA(DisplayName = "None"),
	EHitDirection_Left	UMETA(DisplayName = "Left"),
	EHitDirection_Right		UMETA(DisplayName = "Right"),
	EHitDirection_Front	UMETA(DisplayName = "Front"),
	EHitDirection_Back	UMETA(DisplayName = "Back")
};




