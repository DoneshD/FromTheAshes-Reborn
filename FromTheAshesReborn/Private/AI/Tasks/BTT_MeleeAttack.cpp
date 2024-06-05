#include "AI/Tasks/BTT_MeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Characters/EnemyMelee.h"

UBTT_MeleeAttack::UBTT_MeleeAttack()
{
	NodeName = TEXT("BTT_MeleeAttack");
	bCreateNodeInstance = true;

}

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemyMelee* EnemyMelee = Cast<AEnemyMelee>(Pawn);

	AAIController* EnemyController = EnemyMelee->GetController<AAIController>();
	AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());


	EnemyMelee->OnAttackEnd.BindUObject(this, &UBTT_MeleeAttack::FinishedAttacking);

	if (!EnemyMelee)
	{
		return EBTNodeResult::Failed;
	}

	AActor* AttackTarget = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));
	IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(EnemyMelee);

	/*
	if (AIEnemyInterface->AttackStart(AttackTarget, TokensNeeded))
	{

		EnemyMelee->SetMovementSpeed(MovementSpeed);
		EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
		EnemyController->SetFocus(AttackTarget);

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(AttackTarget);
		MoveRequest.SetAcceptanceRadius(OwnerComp.GetBlackboardComponent()->GetValueAsFloat(AttackRadiusKey.SelectedKeyName));

		AIControllerEnemyBase->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UBTT_MeleeAttack::ReachedLocation);

		FPathFollowingRequestResult RequestResult = OwnerComp.GetAIOwner()->MoveTo(MoveRequest);
		if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::InProgress);
			return EBTNodeResult::InProgress;
		}
		else
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	*/
	return EBTNodeResult::Failed;
}

void UBTT_MeleeAttack::ReachedLocation(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());

	if (OwnerComp)
	{
		APawn* Pawn = OwnerComp->GetAIOwner()->GetPawn();
		if (Pawn)
		{
			AEnemyMelee* EnemyMelee = Cast<AEnemyMelee>(Pawn);
			if (EnemyMelee)
			{
				EnemyMelee->LightAttack();
				FinishLatentTask(*OwnerComp, EBTNodeResult::InProgress);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("EnemyMelee NULL"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pawn NULL"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerComp NULL"));
	}
}


void UBTT_MeleeAttack::FinishedAttacking()
{
	UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(GetOuter());

	if (OwnerComp)
	{
		APawn* Pawn = OwnerComp->GetAIOwner()->GetPawn();
		if (Pawn)
		{
			AEnemyMelee* EnemyMelee = Cast<AEnemyMelee>(Pawn);
			if (EnemyMelee)
			{
				AActor* AttackTarget = Cast<AActor>(OwnerComp->GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

				IAIEnemyInterface* AIEnemyInterface = Cast<IAIEnemyInterface>(EnemyMelee);
				if (AIEnemyInterface)
				{
					AAIControllerEnemyBase* AIControllerEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp->GetAIOwner());
					AIControllerEnemyBase->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
					if (AttackTarget)
					{
						//AIEnemyInterface->AttackEnd(AttackTarget);
						FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("NOT AIEnemyInterface"));

				}

			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("EnemyMelee NULL"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pawn NULL"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerComp NULL"));
	}

}