
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GroundedDashingCommponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UGroundedDashingCommponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UGroundedDashingCommponent();

public:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
