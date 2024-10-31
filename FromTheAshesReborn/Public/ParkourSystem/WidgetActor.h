#pragma once

#include "CoreMinimal.h"
#include "ParkourStatsInterface.h"
#include "GameFramework/Actor.h"
#include "WidgetActor.generated.h"

class UWidgetComponent;

UCLASS()
class FROMTHEASHESREBORN_API AWidgetActor : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour System|Widget")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour System|Widget")
	TObjectPtr<UWidgetComponent> WidgetComponent;
	
	AWidgetActor();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	//----Interfaces-----

	// UFUNCTION()
	// virtual bool SetParkourState(FText ParkourState) override;
	//
	// UFUNCTION()
	//
	// virtual bool SetParkourAction(FText ParkourAction) override;
	//
	// UFUNCTION()
	//
	// virtual bool SetClimbStyle(FText ClimbStyle) override;
};
