#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/AbilityTasks/AT_WaitInputQueueAndInputWindow.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "ComboManagerComponent.generated.h"

class UFTAAbilityDataAsset;
class UFTAAbilitySystemComponent;
class UGameplayAbility;
class UFTAGameplayAbility;
class AFTAPlayerController;
class UAbilitySystemComponent;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRegisterWindowTagEventSignature, FGameplayTag, ComboWindowTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRegisterTestWindowTagEventSignature, FGameplayTag, TestComboWindowTag);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UComboManagerComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	
	UFTAAbilitySystemComponent* FTAASC;
	
	FGameplayTagContainer CurrentComboTagContainer;
	int CurrentComboIndex = 0;
	
	TMap<FGameplayTag, FDelegateHandle> TagDelegateHandles;
	TMap<FGameplayTag, FTimerHandle> TagTimerHandles;
	
	UComboManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	void ComboWindowOpen(FGameplayTag ComboWindowTag);
	void ProceedToNextAbility(TSubclassOf<UGameplayAbility> AbilityToActivateClass);

	
	void RemoveGameplayTagEvent(FGameplayTag ComboWindowTag);

public:

	// FRegisterWindowTagEventSignature OnRegisterWindowTagEventDelegate;
	FRegisterTestWindowTagEventSignature OnRegisterTestWindowTagEventDelegate;

	UPROPERTY(EditAnywhere)
	bool IsInInputQueueWindow = false;

	FRegisterInputWindowTagEventSignature OnRegisterInputWindowTagEventDelegate;
	
	UFUNCTION(BlueprintCallable)
	void InputQueueAllowedInputsBegin(TArray<TSubclassOf<UFTAGameplayAbility>> QueueableAbilityClasses);

	UFUNCTION(BlueprintCallable)
	void InputQueueUpdateAllowedInputsEnd();
	
	FGameplayTagContainer& GetCurrentComboContainer();

	int GetCurrentComboIndex();

	void SetCurrentComboIndex(int Index);
	
	UFUNCTION()
	void RegisterGameplayTagEvent(FGameplayTag ComboWindowTag);

	bool FindMatchingAssetToTagContainer(const TArray<UFTAAbilityDataAsset*>& AbilityDataAssets, TObjectPtr<UFTAAbilityDataAsset>& OutMatchingAbilityDataAsset);
	
};