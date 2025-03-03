#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GA_Jump.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_Jump : public UFTAGameplayAbility
{
	GENERATED_BODY()

private:
	int JumpCount = 0;
	
	ACharacter* CharacterRef;

public:
	UGA_Jump();

	UFUNCTION()
	void OnCharacterLanded(const FHitResult& Hit);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};