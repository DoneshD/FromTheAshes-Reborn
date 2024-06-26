
#include "Ai/Decorators/BTDecorator_IsWithinIdealRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"


UBTDecorator_IsWithinIdealRange::UBTDecorator_IsWithinIdealRange()
{
	NodeName = TEXT("BTD_IsWithinIdealRange");
	bNotifyTick = true;
}

void UBTDecorator_IsWithinIdealRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	bool bWithinIdealRange = CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (!bWithinIdealRange)
	{
		OwnerComp.RequestExecution(this);
	}
	
}

bool UBTDecorator_IsWithinIdealRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AActor* Enemy = OwnerComp.GetAIOwner()->GetPawn();
	AActor* AttackTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	float Distance = Enemy->GetDistanceTo(AttackTarget) - ErrorMargin;
	float IdealRange = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName);

	return Distance <= IdealRange;
	
}
