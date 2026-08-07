#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponent.h"
#include "FTAStateTreeAIComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UFTAStateTreeAIComponent : public UStateTreeAIComponent
{
	GENERATED_BODY()

protected:
	UFTAStateTreeAIComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	void SetStateTree(UStateTree* InStateTree);
};
