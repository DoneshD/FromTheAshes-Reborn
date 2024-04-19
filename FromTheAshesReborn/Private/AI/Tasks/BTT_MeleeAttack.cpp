#include "AI/Tasks/BTT_MeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/EnemyMelee.h"


UBTT_MeleeAttack::UBTT_MeleeAttack()
{
	NodeName = TEXT("BTT_MeleeAttack");
}

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	EnemyOwnerComp = &OwnerComp;

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyMelee* EnemyMelee = Cast<AEnemyMelee>(Pawn);

	EnemyMelee->OnAttackEnd.BindUObject(this, &UBTT_MeleeAttack::FinishedAttacking);

	if (!EnemyMelee)
	{
		return EBTNodeResult::Failed;
	}

	AActor* AttackTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	AAIController* EnemyController = EnemyMelee->GetController<AAIController>();
	AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());

	AIControllerEnemyBase->OnMoveCompletedDelegate.BindUObject(this, &UBTT_MeleeAttack::ReachedLocation);

	EnemyMelee->SetMovementSpeed(MovementSpeed);
	EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
	EnemyController->SetFocus(AttackTarget);

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(AttackTarget);
	MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName));

	FPathFollowingRequestResult RequestResult = OwnerComp.GetAIOwner()->MoveTo(MoveRequest);
	if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("InProgress"));
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::InProgress;
}

void UBTT_MeleeAttack::ReachedLocation()
{
	if (EnemyOwnerComp)
	{
		UBehaviorTreeComponent& OwnerComp = *EnemyOwnerComp;

		APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
		AEnemyMelee* EnemyMelee = Cast<AEnemyMelee>(Pawn);
		AActor* AttackTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

		AAIController* EnemyController = EnemyMelee->GetController<AAIController>();
		AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());
		if (AIControllerEnemyBase->GetCurrentState() == EAIStates::EAIStates_Attacking)
		{
			IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(EnemyMelee);
			if (AIEnemyInterface->AttackStart(AttackTarget, TokensNeeded))
			{
				UE_LOG(LogTemp, Warning, TEXT("Attack"));
				EnemyMelee->LightAttack();
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT Attacking"));
		}

	}
}

void UBTT_MeleeAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	UE_LOG(LogTemp, Warning, TEXT("Desolation"));

	
}

void UBTT_MeleeAttack::FinishedAttacking()
{
	if (EnemyOwnerComp)
	{
		UBehaviorTreeComponent& OwnerComp = *EnemyOwnerComp;

		APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
		AEnemyMelee* EnemyMelee = Cast<AEnemyMelee>(Pawn);
		AActor* AttackTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

		AAIController* EnemyController = EnemyMelee->GetController<AAIController>();
		AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());

		IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(EnemyMelee);
		if (AIEnemyInterface)
		{
			AIEnemyInterface->AttackEnd(AttackTarget);
		}

	}
	FinishLatentTask(*EnemyOwnerComp, EBTNodeResult::Succeeded);
}