#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CentralStateComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCentralStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:

protected:
	
	UCentralStateComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	
};
