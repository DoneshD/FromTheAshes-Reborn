﻿#pragma once

#include "CoreMinimal.h"
#include "GA_ReceiveHit.h"
#include "GA_AerialSuspension.generated.h"

class UAerialCombatComponent;
class UAT_SuspendInAirAndWait;

UCLASS()
class FROMTHEASHESREBORN_API UGA_AerialSuspension : public UGA_ReceiveHit
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TObjectPtr<UAT_SuspendInAirAndWait> SuspendTask;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY()
	TObjectPtr<UAerialCombatComponent> AerialCombatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE")
	TSubclassOf<UGameplayEffect> EnableAerialCombatEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE")
	TSubclassOf<UGameplayEffect> AddAerialCombatGravity;

public:

	UGA_AerialSuspension();

	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnSuspendComplete();
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
};
