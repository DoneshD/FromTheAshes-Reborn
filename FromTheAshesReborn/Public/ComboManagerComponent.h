#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "ComboManagerComponent.generated.h"

class UMeleeAbilityDataAsset;
class UFTAAbilityDataAsset;
class UFTAAbilitySystemComponent;
class UFTAGameplayAbility;
class AFTAPlayerController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UComboManagerComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	
	TObjectPtr<UFTAAbilitySystemComponent> FTAASC;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer CurrentComboTagContainer;

	UPROPERTY(BlueprintReadWrite)
	int CurrentComboIndex = 0;

protected:
	
	UComboManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	
	FGameplayTagContainer& GetCurrentComboContainer();
	int GetCurrentComboIndex() const;
	void SetCurrentComboIndex(int Index);
	
	bool FindMatchingAssetToTagContainer(const TArray<UMeleeAbilityDataAsset*>& AbilityDataAssets, TObjectPtr<UMeleeAbilityDataAsset>& OutMatchingAbilityDataAsset);
	
};