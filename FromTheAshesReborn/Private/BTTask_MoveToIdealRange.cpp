// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToIdealRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "Characters/EnemyBase.h"


UBTTask_MoveToIdealRange::UBTTask_MoveToIdealRange()
{
    NodeName = TEXT("BTTask_MoveToIdealRange");

}

EBTNodeResult::Type UBTTask_MoveToIdealRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
    APawn* TargetActor = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    float AcceptanceRadius = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName);

    UAIAsyncTaskBlueprintProxy* MoveToProxy = UAIBlueprintHelperLibrary::CreateMoveToProxyObject
	(GetWorld(), OwnerPawn, TargetActor->GetActorLocation(), TargetActor, AcceptanceRadius);

    if (MoveToProxy)
    {
        // Optionally, you can bind to delegate functions for completion, success, or failure
		MoveToProxy->OnSuccess.
    }



    return EBTNodeResult::Succeeded;
}