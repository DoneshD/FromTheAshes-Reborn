#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FTAInputConfig.generated.h"

class UInputAction;

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FFTAInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FFTAInputAction> AbilityInputActions;
};

