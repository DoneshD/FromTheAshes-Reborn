#include "AI/Tasks/BTT_MeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/EnemyBase.h"


UBTT_MeleeAttack::UBTT_MeleeAttack()
{
	NodeName = TEXT("BTT_MeleeAttack");
}

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	EnemyOwnerComp = &OwnerComp;

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* Enemy = Cast<AEnemyBase>(Pawn);

	Enemy->OnAttackEnd.BindUObject(this, &UBTT_MeleeAttack::FinishedAttacking);

	if (!Enemy)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	AAIController* EnemyController = Enemy->GetController<AAIController>();

	//Start Attack interface function
	if (Enemy->AttackStart(TargetActor, TokensNeeded))
	{
		//Set the movement speed
		Enemy->SetMovementSpeed(MovementSpeed);

		//Clear focus
		EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
	
		//Move to the ideal range

		//FAIRequestID RequestID;
		//EnemyController->OnMoveCompleted(RequestID, EPathFollowingResult::Success);
		//EnemyController->OnMoveCompleted.AddDynamic(this, &UBTT_MeleeAttack::OnMoveCompleted);

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(TargetActor);
		MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName));
		float AttackRadius = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(AttackRadiusKey.SelectedKeyName);

		EnemyController->MoveToActor(TargetActor, AttackRadius);

		//Set focus
		EnemyController->SetFocus(TargetActor);

		//Attack
		Enemy->LightAttack();

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTT_MeleeAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyBase* EnemyBase = Cast<AEnemyBase>(Pawn);

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(TargetActor);

	if (DamagableInterface)
	{
		DamagableInterface->ReturnAttackToken(1);
	}

	IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(EnemyBase);
	if (AIEnemyInterface)
	{
		UE_LOG(LogTemp, Warning, TEXT("Keean!@#$@$!"));
		AIEnemyInterface->AttackEnd(TargetActor);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTT_MeleeAttack::FinishedAttacking()
{	
	if (EnemyOwnerComp)
	{
		OnTaskFinished(*EnemyOwnerComp, nullptr, EBTNodeResult::Succeeded);
	}
}