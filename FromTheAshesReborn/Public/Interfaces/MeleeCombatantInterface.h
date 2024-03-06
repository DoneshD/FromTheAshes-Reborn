#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MeleeCombatantInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMeleeCombatantInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IMeleeCombatantInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void EmptyHitActorsArray() = 0;

	UFUNCTION()
	virtual void StartMeleeAttackCollisions() = 0;

	UFUNCTION()
	virtual void EndMeleeAttackCollisions() = 0;

	UFUNCTION()
	virtual bool MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits) = 0;

	UFUNCTION()
	virtual void MeleeTraceCollisions() = 0;

	UFUNCTION()
	virtual void MeleeAttackWarpToTarget(FMotionWarpingTarget& MotionWarpingTargetParams) = 0;

	UFUNCTION()
	virtual void ResetMeleeAttackWarpToTarget() = 0;

};
