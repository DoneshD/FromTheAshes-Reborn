#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyCharacterDataAsset.generated.h"

class UAICombatParameters;
class UStateTree;
class UStateTreeComponentSchema;
class UStateTreeSchema;
class UStateTreeAIComponent;
class UWeaponDefinition;
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UWeaponDefinition> WeaponDefinitionClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UStateTree> StateTree;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAICombatParameters> CombatParameters;
	
};
