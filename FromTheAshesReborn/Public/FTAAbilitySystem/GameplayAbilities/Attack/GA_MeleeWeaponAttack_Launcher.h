﻿#pragma once

#include "CoreMinimal.h"
#include "GA_MeleeWeaponAttack.h"
#include "EventObjects/LaunchEventObject.h"
#include "GA_MeleeWeaponAttack_Launcher.generated.h"

class UAT_LaunchCharacterAndWait;

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack_Launcher : public UGA_MeleeWeaponAttack
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE")
	TSubclassOf<UGameplayEffect> EnableAerialCombatEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Launcher Ability")
	float LauncherVerticalDistance = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Launcher Ability")
	float LauncherDuration = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launcher Ability")
	float StallDuration = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launcher Ability")
	float LaunchOffset = 0.0f;

	UPROPERTY()
	TObjectPtr<UAT_LaunchCharacterAndWait> LaunchTask;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launcher Ability")
	FGameplayTagContainer QueueableAbilities;
	

protected:
	
	UGA_MeleeWeaponAttack_Launcher();
	
	virtual void OnAbilityTick(float DeltaTime) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

	UFUNCTION()
	void OnLaunchComplete();
	
};