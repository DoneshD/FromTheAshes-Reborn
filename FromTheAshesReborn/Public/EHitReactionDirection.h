// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) enum class EHitReactionDirection : uint8
{
	EHitReactionDirection_None		UMETA(DisplayName = "None"),
	EHitReactionDirection_Left	UMETA(DisplayName = "Left"),
	EHitReactionDirection_Right		UMETA(DisplayName = "Right"),
	EHitReactionDirection_Front	UMETA(DisplayName = "Front"),
	EHitReactionDirection_Back	UMETA(DisplayName = "Back")
};
