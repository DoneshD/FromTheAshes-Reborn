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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Lurch")
	float JumpLurchStrength = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Lurch")
	float JumpLurchVelocity = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Lurch")
	float JumpLurchMaxMultiplier = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Lurch")
	float JumpLurchMinMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Lurch")
	float JumpLurchSpeedLoss = 12.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Lurch")
	float JumpLurchWindowDuration = 0.5f;
	
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

	void UpdateJumpLurchStrength();
	
};