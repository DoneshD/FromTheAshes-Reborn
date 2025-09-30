#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPropertiesComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UAIPropertiesComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	
	UAIPropertiesComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
