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
    /*
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

    AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());
    if (!EnemyController)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyController is NULL"));
        return EBTNodeResult::Failed;
    }


    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(AttackTarget);
    MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName));

    EnemyBase->SetMovementSpeed(MovementSpeed);
    EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
    EnemyController->SetFocus(AttackTarget);

    FPathFollowingRequestResult RequestResult = OwnerComp.GetAIOwner()->MoveTo(MoveRequest);

    if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
    {
        EnemyController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UBTTask_MoveToIdealRange::ReachedLocation);
        FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
        return EBTNodeResult::InProgress;
    }

    */
    return EBTNodeResult::Failed;
}

    /*
void UBTTask_MoveToIdealRange::ReachedLocation(FAIRequestID RequestID, const FPathFollowingResult& Result)
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
            UE_LOG(LogTemp, Warning, TEXT("EnemyController is NULL"));
        }
    }
}
    */

