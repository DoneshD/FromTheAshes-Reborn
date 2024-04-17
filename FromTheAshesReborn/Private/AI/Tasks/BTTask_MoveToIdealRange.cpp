// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_MoveToIdealRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/EnemyBase.h"


UBTTask_MoveToIdealRange::UBTTask_MoveToIdealRange()
{
    NodeName = TEXT("BTTask_MoveToIdealRange");
}

EBTNodeResult::Type UBTTask_MoveToIdealRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    EnemyOwnerComp = &OwnerComp;

    APawn* EnemyPawn = OwnerComp.GetAIOwner()->GetPawn();
    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());
    
    AIControllerEnemyBase->OnMoveCompletedDelegate.BindUObject(this, &UBTTask_MoveToIdealRange::ReachedLocation);

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(TargetActor);
    MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName));


    FPathFollowingRequestResult RequestResult = OwnerComp.GetAIOwner()->MoveTo(MoveRequest);
    if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("InProgress"));
        return EBTNodeResult::InProgress;
    }
    return EBTNodeResult::InProgress;

}

void UBTTask_MoveToIdealRange::ReachedLocation()
{
    if (EnemyOwnerComp)
    {
        FinishLatentTask(*EnemyOwnerComp, EBTNodeResult::Succeeded);
    }
}

