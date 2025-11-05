#pragma once

#include "CoreMinimal.h"
#include "GA_ReceiveHit.h"
#include "GA_Bounce.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API UGA_Bounce : public UGA_ReceiveHit
{
	GENERATED_BODY()

protected:

	bool TraceForFloor = false;

public:
	
	UGA_Bounce();

	void CheckGroundBelow();
	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;

	virtual void OnMontageBlendingOut(FGameplayTag EventTag, FGameplayEventData EventData) override;
	
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

};
