#pragma once

#include "CoreMinimal.h"
#include "GA_GroundedMeleeAttack.h"
#include "GA_GroundedHeavyMeleeAttack.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_GroundedHeavyMeleeAttack : public UGA_GroundedMeleeAttack
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UMeleeAttackDataAsset>> HeavyAttackDataAssets;

	TObjectPtr<UAnimMontage> AttackMontageToPlay;
	

public:
	UGA_GroundedHeavyMeleeAttack();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void PlayAttackMontage(TObjectPtr<UAnimMontage> AttackMontage) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

};
