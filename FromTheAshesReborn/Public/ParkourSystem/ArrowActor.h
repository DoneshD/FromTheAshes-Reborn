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

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parkour System|Arrow")
	TObjectPtr<UArrowComponent> ArrowComponent;
	
	AArrowActor();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
};
