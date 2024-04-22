// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_MoveToStrafePoint.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Characters/EnemyBase.h"


UBTTask_MoveToStrafePoint::UBTTask_MoveToStrafePoint()
{
	NodeName = TEXT("BTTask_MoveToStrafePoint");

}

EBTNodeResult::Type UBTTask_MoveToStrafePoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EnemyOwnerComp = &OwnerComp;

    APawn* EnemyPawn = OwnerComp.GetAIOwner()->GetPawn();
    AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());

    AIControllerEnemyBase->OnMoveCompletedDelegate.BindUObject(this, &UBTTask_MoveToStrafePoint::ReachedLocation);

    FAIMoveRequest MoveRequest;
    FVector Destination = OwnerComp.GetBlackboardComponent()->GetValueAsVector(PointOfInterestKey.SelectedKeyName);
    MoveRequest.SetGoalLocation(Destination);
    //MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(PointOfInterestKey.SelectedKeyName));

    FPathFollowingRequestResult RequestResult = OwnerComp.GetAIOwner()->MoveTo(MoveRequest);
    if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("UBTTask_MoveToStrafePoint::InProgress"));
        return EBTNodeResult::InProgress;
    }
    return EBTNodeResult::InProgress;
}

void UBTTask_MoveToStrafePoint::ReachedLocation(int32 t)
{
    UE_LOG(LogTemp, Warning, TEXT("ReachedLocation: %d"), t);

    if (EnemyOwnerComp)
    {
        FinishLatentTask(*EnemyOwnerComp, EBTNodeResult::Succeeded);
    }

}
