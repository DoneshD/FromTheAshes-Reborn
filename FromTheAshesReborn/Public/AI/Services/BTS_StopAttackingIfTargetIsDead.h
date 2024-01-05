// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_StopAttackingIfTargetIsDead.generated.h"

/**
 * 
 */
UCLASS()
class FROMTHEASHESREBORN_API UBTS_StopAttackingIfTargetIsDead : public UBTService_BlackboardBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackTargetKey;

public:

	UBTS_StopAttackingIfTargetIsDead();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
