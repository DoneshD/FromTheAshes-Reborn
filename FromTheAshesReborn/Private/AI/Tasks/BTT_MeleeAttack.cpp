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

	AAIController* EnemyController = EnemyMelee->GetController<AAIController>();
	AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());
	AIControllerEnemyBase->OnMoveCompletedDelegate.BindUObject(this, &UBTT_MeleeAttack::ReachedLocation);
	EnemyMelee->OnAttackEnd.BindUObject(this, &UBTT_MeleeAttack::FinishedAttacking);

	if (!EnemyMelee)
	{
		return EBTNodeResult::Failed;
	}

	AActor* AttackTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(EnemyMelee);
	if (AIEnemyInterface->AttackStart(AttackTarget, 1))
	{
		AIControllerEnemyBase->SetStateAsHostile();
		EnemyMelee->SetMovementSpeed(MovementSpeed);
		EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
		EnemyController->SetFocus(AttackTarget);

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(AttackTarget);
		MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName));

		FPathFollowingRequestResult RequestResult = OwnerComp.GetAIOwner()->MoveTo(MoveRequest);
		if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
		{
			return EBTNodeResult::InProgress;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Token missing"));
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
		
		EnemyMelee->LightAttack();
	}
}

void UBTT_MeleeAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	
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
			AIControllerEnemyBase->SetStateAsAttacking(AttackTarget, true);
			AIEnemyInterface->AttackEnd(AttackTarget);
		}

	}
	
	FinishLatentTask(*EnemyOwnerComp, EBTNodeResult::Succeeded);
}