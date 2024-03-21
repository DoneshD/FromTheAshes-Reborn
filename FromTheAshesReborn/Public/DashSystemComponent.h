#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashSystemComponent.generated.h"


class UArrowComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UDashSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	TObjectPtr<APlayableCharacter> PC;

	TObjectPtr<UArrowComponent> DashWarpTargetArrow;

	bool EnableLateralDash = false;

	UPROPERTY(BlueprintReadWrite)
	bool IsDashSaved = false;

protected:

public:	

	float GetAngleOfDash();

	bool InRangeOfLateralDash();

	void LockOnDash();

	void FreeLockDash();

	UFUNCTION(BlueprintCallable)
	void SaveDash();

	void DashWarpToTarget();

	void ResetDashWarpToTarget();

	UDashSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
