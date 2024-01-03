// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetStateAsPassive.h"
#include "AIControllerEnemyBase.h"


UBTTask_SetStateAsPassive::UBTTask_SetStateAsPassive()
{
	NodeName = TEXT("BTT_SetStateAsPassive");
}	

EBTNodeResult::Type UBTTask_SetStateAsPassive::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIControllerEnemyBase* AICEnemyBase = Cast<AAIControllerEnemyBase>(OwnerComp.GetAIOwner());
	if (AICEnemyBase)
	{
		AICEnemyBase->SetStateAsPassive();
	}
	return EBTNodeResult::Succeeded;
}
