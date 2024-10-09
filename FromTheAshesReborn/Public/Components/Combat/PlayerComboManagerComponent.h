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
	
	FGameplayTagContainer CurrentComboTagContainer;

	int CurrentComboIndex = 0;

	UAbilitySystemComponent* ASComponent;

	AFTAPlayerController* PC;

	TMap<FGameplayTag, FDelegateHandle> TagEventHandles;

public:
	
	UPlayerComboManagerComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrintCurrentComboContainer();

	FGameplayTagContainer& GetCurrentComboContainer();

	int GetCurrentComboIndex();

	void SetCurrentComboIndex(int Index);
	
	virtual void ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount, FGameplayTag ComboWindowTag, FTimerHandle& ComboWindowTimer);

	void ComboWindowOpen(FGameplayTag ComboWindowTag, FTimerHandle& ComboWindowTimer);

	void RegisterGameplayTagEvent(FGameplayTag& ComboWindowTag, FTimerHandle& FComboWindowTimer);

	void RemoveGameplayTagEvent(FGameplayTag& ComboWindowTag, FTimerHandle& FComboWindowTimer);
	
	void ProceedNextAbility(int GameplayAbilityInputID);
};
