#pragma once

#include "CoreMinimal.h"
#include "FTAGameplayAbility.h"
#include "GA_AerialHit_Suspended.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_AerialHit_Suspended : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:

	bool bDescend = false; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Air Suspend Ability")
	float DescentSpeed = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Air Suspend Ability")
	float SuspendTimerSpeed = 0.1;
	
	FTimerHandle AerialSuspensionTimer;

public:

	UGA_AerialHit_Suspended();

	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void UpdateAerialDescentMovement(float DeltaTime);
	
	void EndAirStall();
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
};
