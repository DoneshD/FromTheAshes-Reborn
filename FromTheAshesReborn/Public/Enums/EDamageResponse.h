// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) enum class EDamageResponse : uint8
{
	EDamageResponse_None		UMETA(DisplayName = "None"),
	EDamageResponse_HitReaction	UMETA(DisplayName = "HitReaction"),
	EDamageResponse_Stagger		UMETA(DisplayName = "Stagger"),
	EDamageResponse_Stun		UMETA(DisplayName = "Stun"),
	EDamageResponse_Knockback	UMETA(DisplayName = "Knockback")
};
