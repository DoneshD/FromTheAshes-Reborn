// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_ClearFocus.h"
#include "AIController.h"

UBTTask_ClearFocus::UBTTask_ClearFocus()
{
	NodeName = TEXT("BTT_ClearFocus");

}

EBTNodeResult::Type UBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);
	return EBTNodeResult::Succeeded;
}
