#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArrowActor.generated.h"

class UArrowComponent;

UCLASS()
class FROMTHEASHESREBORN_API AArrowActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour System|Arrow")
	TObjectPtr<UArrowComponent> ArrowComponent;

public:
	AArrowActor();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
};
