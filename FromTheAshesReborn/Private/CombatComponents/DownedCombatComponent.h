#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DownedCombatComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UDownedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDownedCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
