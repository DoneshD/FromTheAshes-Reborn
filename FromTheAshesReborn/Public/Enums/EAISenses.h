// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) enum class EAISenses : uint8
{

	EAISenses_None		UMETA(DisplayName = "None"),
	EAISenses_Sight		UMETA(DisplayName = "Sight"),
	EAISenses_Hearing	UMETA(DisplayName = "Hearing"),
	EAISenses_Damage	UMETA(DisplayName = "Damage"),

};
