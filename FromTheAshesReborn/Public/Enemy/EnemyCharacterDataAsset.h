#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateTreeReference.h"
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

USTRUCT(BlueprintType)
struct FStateTreeLinkedAsset
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStateTree* StateTree;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag AssetTag = FGameplayTag::EmptyTag;
	
};

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State Tree")
	TObjectPtr<UAICombatParameters> CombatParameters;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State Tree")
	TObjectPtr<UStateTree> BaseCombatStateTree;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State Tree")
	FStateTreeLinkedAsset PassiveTree;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State Tree")
	FStateTreeLinkedAsset ActiveTree;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State Tree")
	FStateTreeLinkedAsset AttackTree;
	
};
