#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GA_Jump.generated.h"

class UCharacterMovementComponent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_Jump : public UFTAGameplayAbility
{
	GENERATED_BODY()

private:
	
	int JumpCount = 0;
	
	ACharacter* CharacterRef;
	UCharacterMovementComponent* CMC;

	FTimerHandle FJumpLurchWindowTimerHandle;

protected:
	
	float CurrentJumpLurchStrengthMultiplier = 0.0f;
	float CounterCurrentJumpLurchStrengthMultiplier = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Jump Lurch | ")
	float JumpLurchBaseStrength = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "Jump Lurch")
	float JumpLurchMaxMultiplier = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Jump Lurch")
	float JumpLurchMinMultiplier = 10.0f;
	
	UPROPERTY(EditAnywhere, Category = "Jump Lurch")
	float JumpLurchWindowDuration = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Jump Lurch")
	float CounterLurchMaxMultiplier = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Jump Lurch")
	float CounterLurchMinMultiplier = 10.0f;
	
	float JumpLurchElapsedTime = 0.0f;

public:
	
	UGA_Jump();
	virtual void AbilityTickComponent() override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnCharacterLanded(const FHitResult& Hit);

	void StartJumpLurchWindowTimer();

	void UpdateJumpLurch();

	void UpdateJumpLurchStrength();
	void UpdateCounterLurchForce();
};