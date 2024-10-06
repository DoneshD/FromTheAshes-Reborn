#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "PlayerComboManagerComponent.generated.h"

class AFTAPlayerController;
class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UPlayerComboManagerComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	
	FGameplayTag LightInputSavedTag = FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light"));
	FGameplayTag HeavyInputSavedTag = FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Heavy"));
	FGameplayTag DashInputSavedTag = FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Dash"));
	
	FGameplayTag DashComboWindowTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.ComboWindow.Open.Dash"));
	FGameplayTag LightComboWindowTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.ComboWindow.Open.Light"));
	FGameplayTag HeavyComboWindowTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.ComboWindow.Open.Heavy"));

	// FGameplayTag SavedInputTag;
	// FGameplayTag ComboWindowTag;
	// FTimerHandle ComboWindowTimer;
	//
	// FTimerHandle FLightComboWindowTimer;
	// FTimerHandle FHeavyComboWindowTimer;
	// FTimerHandle FDashComboWindowTimer;

	// FGameplayTag WindowTag;
	// FGameplayTag ComboWindowTag;
	// FTimerHandle FComboWindowTimer;
	FGameplayTagContainer CurrentComboTagContainer;

	int CurrentComboIndex = 0;

	UAbilitySystemComponent* ASComponent;

	AFTAPlayerController* PC;

public:
	
	UPlayerComboManagerComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrintCurrentComboContainer();

	FGameplayTagContainer& GetCurrentComboContainer();

	int GetCurrentComboIndex();

	void SetCurrentComboIndex(int Index);

	// virtual void LightComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	// virtual void HeavyComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	// virtual void DashWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	// void LightComboWindowOpen();
	// void HeavyComboWindowOpen();
	// void DashWindowTagOpen();
	
	virtual void ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount, FGameplayTag InputSavedTag, FGameplayTag ComboWindowTag, FTimerHandle ComboWindowTimer);

	void ComboWindowOpen(FGameplayTag InputToCheck);

	void RegisterGameplayTagEvent(FGameplayTag InputSavedTag, FGameplayTag ComboWindowTag, FTimerHandle FComboWindowTimer);
	
};
