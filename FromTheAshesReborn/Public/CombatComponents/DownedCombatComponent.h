#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DownedCombatComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UDownedCombatComponent : public UActorComponent
{
	GENERATED_BODY()


protected:
	
	UDownedCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
