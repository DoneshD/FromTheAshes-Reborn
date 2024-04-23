#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "AIController.h"
#include "BTT_MeleeAttack.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UBTT_MeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackRadiusKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector IdealRangeKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementSpeed MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tokens")
	int TokensNeeded;


public:

	UBTT_MeleeAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void ReachedLocation(FAIRequestID RequestID, const FPathFollowingResult& Result);
	
	void FinishedAttacking();

};
