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
	AEnemyMelee();

	virtual void NativeGetIdealRange(float& OutAttackRadius, float& OutDefendRadius) override;
};
