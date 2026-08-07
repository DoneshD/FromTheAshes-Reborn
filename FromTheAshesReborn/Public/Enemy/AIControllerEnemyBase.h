#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerEnemyBase.generated.h"

class UFTAStateTreeAIComponent;

UCLASS()
class FROMTHEASHESREBORN_API AAIControllerEnemyBase : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> PlayerTarget;

	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UFTAStateTreeAIComponent> StateTreeComponent;

protected:
	
	AAIControllerEnemyBase(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	
};
