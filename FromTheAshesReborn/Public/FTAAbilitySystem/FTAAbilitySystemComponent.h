#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FTAAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UFTAAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFTAAbilitySystemComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
