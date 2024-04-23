// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_MoveToStrafePoint.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/EnemyBase.h"


UBTTask_MoveToStrafePoint::UBTTask_MoveToStrafePoint()
{
    NodeName = TEXT("BTTask_MoveToStrafePoint");
    bCreateNodeInstance = true;

}

EBTNodeResult::Type UBTTask_MoveToStrafePoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());
    if (!AIControllerEnemyBase)
    {
        return EBTNodeResult::Failed;
    }



    FAIMoveRequest MoveRequest;
    FVector Destination = OwnerComp.GetBlackboardComponent()->GetValueAsVector(PointOfInterestKey.SelectedKeyName);
    MoveRequest.SetGoalLocation(Destination);

    FPathFollowingRequestResult RequestResult = AIControllerEnemyBase->MoveTo(MoveRequest);

    if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
    {
        AIControllerEnemyBase->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UBTTask_MoveToStrafePoint::ReachedLocation);

        FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
        return EBTNodeResult::InProgress;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RequestResult failed"));
        //AIControllerEnemyBase->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);

        return EBTNodeResult::Failed;

    }
    //AIControllerEnemyBase->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
    return EBTNodeResult::Failed;
}

void UBTTask_MoveToStrafePoint::ReachedLocation(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    UE_LOG(LogTemp, Warning, TEXT("ReachedLocation from strafing"));

    UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());
    if (OwnerComp)
    {
        AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp->GetAIOwner());
        AIControllerEnemyBase->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
        FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
    }
}
