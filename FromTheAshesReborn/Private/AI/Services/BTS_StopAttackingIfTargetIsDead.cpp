// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/BTS_StopAttackingIfTargetIsDead.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "Characters/EnemyBase.h"

UBTS_StopAttackingIfTargetIsDead::UBTS_StopAttackingIfTargetIsDead()
{
	NodeName = TEXT("BTS_StopAttackingIfTargetIsDead");
}

void UBTS_StopAttackingIfTargetIsDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttackTargetKey.SelectedKeyName));

	if (TargetActor)
	{
		AEnemyBase* EnemyBase = Cast<AEnemyBase>(TargetActor);
		if (EnemyBase)
		{
			if (EnemyBase->NativeIsDead())
			{
				AAIControllerEnemyBase* AICEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());
				if (AICEnemyBase)
				{
					AICEnemyBase->SetStateAsPassive();
				}
			}
			else
			{
			}
		}
	}

}
