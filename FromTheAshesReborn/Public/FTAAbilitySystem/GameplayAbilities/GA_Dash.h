#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GA_Dash.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_Dash : public UFTAGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Dash();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* DashMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* BlinkMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Distance;

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	
	
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
};
