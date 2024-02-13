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

	AActor* AttackTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	AAIController* EnemyController = Enemy->GetController<AAIController>();

	IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(Enemy);

	//Start Attack interface function
	if (AIEnemyInterface->AttackStart(AttackTarget, TokensNeeded))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack Start Succeeded"));
		//Set the movement speed
		Enemy->SetMovementSpeed(MovementSpeed);

		//Clear focus
		EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
	

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(AttackTarget);
		MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(AttackRadiusKey.SelectedKeyName));
	
		float IdealRangeValue = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(IdealRangeKey.SelectedKeyName);
		float AttackRadiusValue = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(AttackRadiusKey.SelectedKeyName);

		float AttackRadius = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(AttackRadiusKey.SelectedKeyName);

		UE_LOG(LogTemp, Warning, TEXT("Ideal Range Value: %f"), IdealRangeValue);
		UE_LOG(LogTemp, Warning, TEXT("Attack Radius Value: %f"), AttackRadiusValue);


		FPathFollowingResult MoveToResult = EnemyController->MoveTo(MoveRequest);
		UE_LOG(LogTemp, Warning, TEXT("MoveTo: %s"), *MoveToResult.ToString());
		if (MoveToResult.Code == EPathFollowingResult::Success)
		{
			//Set focus
			EnemyController->SetFocus(AttackTarget);

			//Attack
			Enemy->LightAttack();

			return EBTNodeResult::Succeeded;
		}
		else if (MoveToResult.Code == EPathFollowingResult::Blocked)
		{
			AIEnemyInterface->AttackEnd(AttackTarget);
			return EBTNodeResult::Failed;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("None"));
		}
	
	}
	UE_LOG(LogTemp, Warning, TEXT("How tf am i here"));

	return EBTNodeResult::Failed;
}

void UBTT_MeleeAttack::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted"));
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