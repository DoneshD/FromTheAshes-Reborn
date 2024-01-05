// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/BTD_CanSeeTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Controllers/AIControllerEnemyBase.h"

UBTD_CanSeeTarget::UBTD_CanSeeTarget()
{
	NodeName = TEXT("BTD_CanSeeTarget");
}

bool UBTD_CanSeeTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	FVector StartLocation = ControlledPawn->GetActorLocation();
	FVector EndLocation = TargetActor->GetActorLocation();

	FCollisionQueryParams QueryParams;
	FCollisionResponseParams ResponseParams;

	QueryParams.AddIgnoredActor(ControlledPawn);

	bool CanSee = GetWorld()->LineTraceTestByChannel(StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, QueryParams, ResponseParams);

	return !CanSee;

}
