#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FTAInputConfig.generated.h"

class UInputAction;

//Defines the input action and associated gameplay tag pairing
USTRUCT(BlueprintType)
struct FFTAInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
	
};

UCLASS(BlueprintType, Const)
class FROMTHEASHESREBORN_API UFTAInputConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFTAInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "FTA | Character")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;

	UFUNCTION(BlueprintCallable, Category = "FTA | Character")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag) const;

public:

	// Note: Meta = (TitleProperty = "InputAction") is a type of categorization in blueprint

	//These input actions must be manually bound, in this instance in the player controller
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FFTAInputAction> NativeInputActions;

	//These will automatically bound based on the matching gameplay tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FFTAInputAction> AbilityInputActions;
};

