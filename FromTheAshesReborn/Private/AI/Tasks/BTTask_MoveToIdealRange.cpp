// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_MoveToIdealRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/EnemyBase.h"


UBTTask_MoveToIdealRange::UBTTask_MoveToIdealRange()
{
    NodeName = TEXT("BTTask_MoveToIdealRange");
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_MoveToIdealRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    APawn* EnemyPawn = OwnerComp.GetAIOwner()->GetPawn();
    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());
    
    
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(TargetActor);
    MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName));


    FPathFollowingRequestResult RequestResult = OwnerComp.GetAIOwner()->MoveTo(MoveRequest);
    if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
    {
        AIControllerEnemyBase->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UBTTask_MoveToIdealRange::ReachedLocation);
        FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
        return EBTNodeResult::InProgress;
    }
    return EBTNodeResult::Failed;
}

void UBTTask_MoveToIdealRange::ReachedLocation(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());
    if (OwnerComp)
    {
        AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp->GetAIOwner());
        AIControllerEnemyBase->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    }
}


