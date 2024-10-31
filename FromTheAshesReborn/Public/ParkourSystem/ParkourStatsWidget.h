// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ParkourStatsInterface.h"
#include "Blueprint/UserWidget.h"
#include "ParkourStatsWidget.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UParkourStatsWidget : public UUserWidget, public IParkourStatsInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour System|Widget")
	FText ParkourStateTexts;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour System|Widget")
	FText ParkourActionText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour System|Widget")
	FText ClimbStyleText;

public:

	UFUNCTION()
	virtual bool SetParkourState(FText ParkourState) override;

	UFUNCTION()
	virtual bool SetParkourAction(FText ParkourAction) override;
	
	UFUNCTION()
	virtual bool SetClimbStyle(FText ClimbStyle) override;

};
