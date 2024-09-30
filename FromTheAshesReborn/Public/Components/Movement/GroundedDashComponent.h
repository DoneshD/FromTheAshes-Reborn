#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GroundedDashComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UGroundedDashComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	
	UGroundedDashComponent();

public:
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
