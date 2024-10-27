#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArrowActor.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AArrowActor : public AActor
{
	GENERATED_BODY()

public:

protected:

public:
	AArrowActor();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
};
