#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "PlayerComboManagerComponent.generated.h"

class UGameplayAbility;
class UFTAGameplayAbility;
class UGA_GroundedDash;
class UGA_GroundedLightMeleeAttack;
class UGA_GroundedHeavyMeleeAttack;
class AFTAPlayerController;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FAbilityComboData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> AbilityComboClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag InputSavedTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ComboWindowTag;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UPlayerComboManagerComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	
	UAbilitySystemComponent* ASComponent;
	
	FGameplayTagContainer CurrentComboTagContainer;
	int CurrentComboIndex = 0;
	
	AFTAPlayerController* PC;

	TMap<FGameplayTag, FDelegateHandle> TagDelegateHandles;
	TMap<FGameplayTag, FTimerHandle> TagTimerHandles;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGA_GroundedLightMeleeAttack> GA_GroundedLightMeleeAttack;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGA_GroundedHeavyMeleeAttack> GA_GroundedHeavyMeleeAttack;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGA_GroundedDash> GA_GroundedDash;

public:

	FAbilityComboData LightAbilityData;
	FAbilityComboData HeavyAbilityData;

	TArray<FAbilityComboData> AbilityComboDataArray;
	
	UPlayerComboManagerComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrintCurrentComboContainer();

	FGameplayTagContainer& GetCurrentComboContainer();

	int GetCurrentComboIndex();

	void SetCurrentComboIndex(int Index);
	
	virtual void ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	void ComboWindowOpen(FGameplayTag ComboWindowTag);

	UFUNCTION()
	void RegisterGameplayTagEvent(FAbilityComboData AbilityComboData);
	
	void RemoveGameplayTagEvent(FGameplayTag ComboWindowTag);

	void ProceedToNextAbility(TSubclassOf<UGameplayAbility> AbilityToActivateClass);
};
