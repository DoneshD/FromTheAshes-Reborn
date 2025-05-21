#pragma once

#include "CoreMinimal.h"
#include "FTAGameplayAbility.h"
#include "GA_Launched_Vertical.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API UGA_Launched_Vertical : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:
	
	bool IsLaunched = false;

	FVector LaunchedStartLocation;
	FVector LaunchedEndLocation;
	
	float LaunchedElapsedTime = 0.0f;
	float LaunchedStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Launched Ability")
	float LaunchedVerticalDistance = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Launched Ability")
	float LaunchedDuration = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Launched Ability")
	float StallDuration = 0.25f;

	FTimerHandle AerialStallTimerHandle;

protected:
	
	UGA_Launched_Vertical();

	virtual void OnAbilityTick(float DeltaTime) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void UpdateLaunchedMovement(float DeltaTime);
	
	void LaunchedLocationReached();

	void EndAirStall();
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
};
