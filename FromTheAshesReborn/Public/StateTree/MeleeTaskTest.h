#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "MeleeTaskTest.generated.h"

struct FGameplayEventData;

USTRUCT()
struct FROMTHEASHESREBORN_API FMeleeTaskTestStruct : public FStateTreeTaskBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "AI")
	FGameplayTag EventTagToSend = FGameplayTag::RequestGameplayTag("Event.MeleeAttack");

	UPROPERTY(EditAnywhere, Category = "AI")
	FGameplayTag TestEventTagToSend;

	UPROPERTY(EditAnywhere, Category = "AI")
	FGameplayTag FinishedEventTag = FGameplayTag::RequestGameplayTag("Ability.State.MeleeAttackFinished");

	UPROPERTY(EditAnywhere, Category = "AI")
	FGameplayTag TestFinishedEventTag;

private:

	FDelegateHandle EventHandle;

	bool bReceivedFinishEvent = false;

public:

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

private:

	void OnGameplayEventReceived(FGameplayEventData Payload);
};
