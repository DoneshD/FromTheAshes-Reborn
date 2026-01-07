#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RangedCombatComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API URangedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	URangedCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
