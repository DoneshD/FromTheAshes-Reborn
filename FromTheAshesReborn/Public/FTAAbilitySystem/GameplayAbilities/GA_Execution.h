#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GA_Execution.generated.h"

class UContextualAnimSceneAsset;

UCLASS()
class UGA_Execution : public UFTAGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TObjectPtr<UContextualAnimSceneAsset>> PossibleExecutions;

	
};
