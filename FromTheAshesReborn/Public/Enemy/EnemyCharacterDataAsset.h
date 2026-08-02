#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyCharacterDataAsset.generated.h"

class AAIControllerEnemyBase;
class AEnemyBaseCharacter;

UCLASS()
class FROMTHEASHESREBORN_API UEnemyCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AEnemyBaseCharacter> EnemyCharacterClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AAIControllerEnemyBase> EnemyControllerClass;
	
};
