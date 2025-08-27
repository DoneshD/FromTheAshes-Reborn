#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatStateComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCombatStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:

protected:
	UCombatStateComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
