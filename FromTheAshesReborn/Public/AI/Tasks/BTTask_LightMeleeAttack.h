#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_LightMeleeAttack.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UBTTask_LightMeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

private:
	UBehaviorTreeComponent* EnemyOwnerComp; 

public:

	UBTTask_LightMeleeAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	void FinishedAttacking();

};