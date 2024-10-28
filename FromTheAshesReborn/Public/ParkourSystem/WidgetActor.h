#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetActor.generated.h"

class UWidgetComponent;

UCLASS()
class FROMTHEASHESREBORN_API AWidgetActor : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour System|Widget")
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour System|Widget")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
public:
	AWidgetActor();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
};
