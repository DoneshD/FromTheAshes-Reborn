#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FTAAbilitySystemBlueprintLibrary.generated.h"

class UFTACueObject;

UCLASS()
class FROMTHEASHESREBORN_API UFTAAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "FTA|Gameplay Effect")
	static UFTACueObject* GetCueObjectFromEffectContext(const FGameplayEffectContextHandle& Context);
	
};
