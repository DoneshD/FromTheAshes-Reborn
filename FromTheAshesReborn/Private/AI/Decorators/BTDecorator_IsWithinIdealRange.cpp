// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/Decorators/BTDecorator_IsWithinIdealRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"


UBTDecorator_IsWithinIdealRange::UBTDecorator_IsWithinIdealRange()
{
	NodeName = TEXT("BTD_IsWithinIdealRange");
}

bool UBTDecorator_IsWithinIdealRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	AActor* Enemy = OwnerComp.GetAIOwner()->GetPawn();
	AActor* AttackTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	float Distance = Enemy->GetDistanceTo(AttackTarget) - ErrorMargin;
	float IdealRange = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName);


	return Distance <= OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName);
	
}
