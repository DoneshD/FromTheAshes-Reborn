#pragma once

#include "CoreMinimal.h"
#include "GA_MeleeWeaponAttack_Aerial.h"
#include "GA_MeleeWeaponAttack_GroundPound.generated.h"

class UAT_SlamCharacterAndWait;

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack_GroundPound : public UGA_MeleeWeaponAttack_Aerial
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GroundPound Ability")
	float GroundPoundSpeed = 6000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GroundPound Ability")
	float TraceVerticalDownwardDistance = 3000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GroundPound Ability")
	float SlamDuration = 0.20f;
	
	bool IsGroundPounding = false;

	FVector TraceStartLocation;
	FVector TraceEndLocation;
	FVector GroundPoundEndLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GroundPound Ability")
	UAnimMontage* SlamMontage;

	UPROPERTY()
	TObjectPtr<UAT_SlamCharacterAndWait> SlamTask;

public:
	
	UGA_MeleeWeaponAttack_GroundPound(const FObjectInitializer& = FObjectInitializer::Get());

	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
	
	virtual void SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;
	void TraceForActors();

	UFUNCTION()
	void SendHitGPEvent(FHitResult HitItemToAdd, FVector LocationEnd, float Speed, float Duration);
	
	UFUNCTION()
	void OnSlamComplete();
	
};
