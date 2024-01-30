#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "EnemyMelee.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AEnemyMelee : public AEnemyBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack")
	TObjectPtr<UAnimMontage> LightAttackAnim;

	AEnemyMelee();

	//Enemy Interface functions

	virtual void NativeGetIdealRange(float& OutAttackRadius, float& OutDefendRadius) override;

	virtual void LightAttack(AActor* AttackTarget) override;
};
