#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashSystemComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UDashSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

protected:

public:	

	void StartDash();

	void DashWarpToTarget(FMotionWarpingTarget& MotionWarpingTargetParams);

	UDashSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
