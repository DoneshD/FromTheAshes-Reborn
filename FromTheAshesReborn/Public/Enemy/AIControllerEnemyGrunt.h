#pragma once

#include "CoreMinimal.h"
#include "AIControllerEnemyBase.h"
#include "AIControllerEnemyGrunt.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AAIControllerEnemyGrunt : public AAIControllerEnemyBase
{
	GENERATED_BODY()

protected:
	
	AAIControllerEnemyGrunt(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
};
