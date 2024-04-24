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

    AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());
    if (!EnemyController)
    {
        return EBTNodeResult::Failed;
    }
    APawn* EnemyPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (!EnemyPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyPawn is NULL"));
        return EBTNodeResult::Failed;
    }

    AEnemyBase* EnemyBase = Cast<AEnemyBase>(EnemyPawn);

    if (!EnemyBase)
    {
        return EBTNodeResult::Failed;
    }

    AActor* AttackTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if (!AttackTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("AttackTarget is NULL"));
        return EBTNodeResult::Failed;
    }

    EnemyController->SetFocus(AttackTarget);
    EnemyBase->SetMovementSpeed(MovementSpeed);

    FAIMoveRequest MoveRequest;
    FVector Destination = OwnerComp.GetBlackboardComponent()->GetValueAsVector(PointOfInterestKey.SelectedKeyName);
    MoveRequest.SetGoalLocation(Destination);

    FPathFollowingRequestResult RequestResult = EnemyController->MoveTo(MoveRequest);

    if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
    {
        EnemyController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UBTTask_MoveToStrafePoint::ReachedLocation);

        FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
        return EBTNodeResult::InProgress;
    }
    else
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return EBTNodeResult::Failed;

    }
    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
    return EBTNodeResult::Failed;
}

void UBTTask_MoveToStrafePoint::ReachedLocation(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());
    if (OwnerComp)
    {
        AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(OwnerComp->GetAIOwner());
        if (EnemyController)
        {
            EnemyController->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
            FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("FinishLatentTask(*OwnerComp, EBTNodeResult::Failed"));
            FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
        }
    }
}
