#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/StateTreeAIComponent.h"
#include "AIControllerEnemyBase.generated.h"

class UAISenseConfig_Damage;

UCLASS()
class FROMTHEASHESREBORN_API AAIControllerEnemyBase : public AAIController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UStateTreeAIComponent> StateTreeComponent;

private:

	AAIControllerEnemyBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAISenseConfig_Damage> AISenseConfigDamage;

protected:
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	
};
