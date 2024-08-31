#pragma once

#include "CoreMinimal.h"
#include "GA_GroundedMeleeAttack.h"
#include "GA_GroundedLightMeleeAttack.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_GroundedLightMeleeAttack : public UGA_GroundedMeleeAttack
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UMeleeAttackDataAsset>> LightAttacks;

	TObjectPtr<UAnimMontage> AttackMontageToPlay;

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;


};
