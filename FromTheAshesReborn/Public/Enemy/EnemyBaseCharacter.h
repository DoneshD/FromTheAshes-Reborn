#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "EnemyBaseCharacter.generated.h"

class UBehaviorTree;

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBaseCharacter : public AFTACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BehaviorTree;


protected:
	
	AEnemyBaseCharacter(const class FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
