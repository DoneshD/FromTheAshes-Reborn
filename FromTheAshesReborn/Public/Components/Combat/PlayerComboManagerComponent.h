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

	TMap<FGameplayTag, FDelegateHandle> TagDelegateHandles;
	TMap<FGameplayTag, FTimerHandle> TagTimerHandles;



public:
	
	UPlayerComboManagerComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrintCurrentComboContainer();

	FGameplayTagContainer& GetCurrentComboContainer();

	int GetCurrentComboIndex();

	void SetCurrentComboIndex(int Index);
	
	virtual void ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount, FGameplayTag ComboWindowTag);

	void ComboWindowOpen(FGameplayTag ComboWindowTag);

	UFUNCTION()
	void RegisterGameplayTagEvent(FGameplayTag ComboWindowTag);

	UFUNCTION()
	void RemoveGameplayTagEvent(FGameplayTag ComboWindowTag);
	
	void ProceedNextAbility(int GameplayAbilityInputID);
};
