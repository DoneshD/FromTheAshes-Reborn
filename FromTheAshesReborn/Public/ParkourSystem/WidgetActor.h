#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetActor.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AWidgetActor : public AActor
{
	GENERATED_BODY()

public:

protected:

public:
	AWidgetActor();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
};
