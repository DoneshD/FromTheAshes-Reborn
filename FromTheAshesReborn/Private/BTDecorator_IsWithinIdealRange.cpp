// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsWithinIdealRange.h"
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

	AActor* TargetActor = OwnerComp.GetAIOwner()->GetPawn();
	AActor* OtherActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	if (!OtherActor || !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error in BTDecorator_IsWithinIdealRange::CalculateRawConditionValue()"));
		return false;
	}

	float Distance = TargetActor->GetDistanceTo(OtherActor);
	return (Distance - ErrorMargin) <= OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName);
	
}
