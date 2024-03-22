#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashSystemComponent.generated.h"


class UArrowComponent;
class UCharacterMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UDashSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	TObjectPtr<APlayableCharacter> PC;

	TObjectPtr<UArrowComponent> DashWarpTargetArrow;

	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	bool EnableLateralDash = false;

	bool IsDashSaved = false;

	bool AlreadyDashed = false;

protected:

public:	

	float GetAngleOfDash();

	bool InRangeOfLateralDash();

	void LockOnDash();

	void FreeLockDash();

	void DashWarpToTarget();

	void ResetDashWarpToTarget();

	UDashSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
