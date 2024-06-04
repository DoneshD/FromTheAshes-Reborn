
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_IsWithinIdealRange.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UBTDecorator_IsWithinIdealRange : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector AttackTargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackboardKey")
	FBlackboardKeySelector IdealRangeKey;

private:

	float ErrorMargin = 50.0f;

public:

	UBTDecorator_IsWithinIdealRange();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;


};
