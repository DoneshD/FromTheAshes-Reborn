#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TagValidationFunctionLibrary.generated.h"


struct FGameplayTagContainer;
struct FGameplayTag;

UCLASS()
class FROMTHEASHESREBORN_API UTagValidationFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsRegisteredGameplayTag(const FGameplayTag& Tag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool AllGameplayTagsRegisteredInContainer(const FGameplayTagContainer& Container);
	
};
