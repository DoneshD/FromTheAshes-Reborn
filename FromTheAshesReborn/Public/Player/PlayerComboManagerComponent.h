#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "PlayerComboManagerComponent.generated.h"

class UFTAAbilitySystemComponent;
class UGameplayAbility;
class UFTAGameplayAbility;
class UGA_GroundedDash;
class UGA_GroundedLightMeleeAttack;
class UGA_GroundedHeavyMeleeAttack;
class AFTAPlayerController;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRegisterWindowTagEventSignature, FGameplayTag, ComboWindowTag);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UPlayerComboManagerComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	
	UFTAAbilitySystemComponent* ASComponent;
	AFTAPlayerController* PC;
	
	FGameplayTagContainer CurrentComboTagContainer;
	int CurrentComboIndex = 0;
	
	TMap<FGameplayTag, FDelegateHandle> TagDelegateHandles;
	TMap<FGameplayTag, FTimerHandle> TagTimerHandles;
	
	UPlayerComboManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	void ComboWindowOpen(FGameplayTag ComboWindowTag);
	void ProceedToNextAbility(TSubclassOf<UGameplayAbility> AbilityToActivateClass);
	
	void RemoveGameplayTagEvent(FGameplayTag ComboWindowTag);

	void PrintCurrentComboContainer();

public:

	FRegisterWindowTagEventSignature OnRegisterWindowTagEventDelegate;

	UPROPERTY(EditAnywhere)
	bool IsInInputQueueWindow = false;
	
	UFUNCTION(BlueprintCallable)
	void InputQueueAllowedInputsBegin(TArray<TSubclassOf<UFTAGameplayAbility>> QueueableAbilityClasses);

	UFUNCTION(BlueprintCallable)
	void InputQueueUpdateAllowedInputsEnd();
	
	FGameplayTagContainer& GetCurrentComboContainer();

	int GetCurrentComboIndex();

	void SetCurrentComboIndex(int Index);
	

	UFUNCTION()
	void RegisterGameplayTagEvent(FGameplayTag ComboWindowTag);

	
};
