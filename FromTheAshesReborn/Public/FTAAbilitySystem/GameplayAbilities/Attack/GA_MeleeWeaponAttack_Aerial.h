#pragma once

#include "CoreMinimal.h"
#include "GA_MeleeWeaponAttack.h"
#include "GA_MeleeWeaponAttack_Aerial.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack_Aerial : public UGA_MeleeWeaponAttack
{
	GENERATED_BODY()

protected:

	bool bDescend = false; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aerial Ability")
	float DescentSpeed = 10;


public:

	UGA_MeleeWeaponAttack_Aerial();

	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void UpdateAerialDescentMovement(float DeltaTime);
	
	void EndAirStall();

	virtual void OnMeleeWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData) override;
	virtual void OnHitAdded(FHitResult LastItem) override;
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

};
