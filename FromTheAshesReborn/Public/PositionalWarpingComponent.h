#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PositionalWarpingComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UPositionalWarpingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	


protected:

public:	

	UPositionalWarpingComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
