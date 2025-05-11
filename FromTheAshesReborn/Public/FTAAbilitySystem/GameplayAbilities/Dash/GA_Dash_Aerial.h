#pragma once

#include "CoreMinimal.h"
#include "GA_Dash.h"
#include "GA_Dash_Aerial.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API UGA_Dash_Aerial : public UGA_Dash
{
	GENERATED_BODY()

protected:

	// Gliding
	FVector GlideVelocity;
	bool IsGliding = false;
	
	float GlideElapsedTime = 0.0f;
	float GlideTimeDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float InitialDownwardVelocity = -100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float InitialForwardGlideSpeed = 2000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float GlideGravityScale = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float GlideDrag = 0.98f;

protected:

	UGA_Dash_Aerial();
	
	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// virtual void UpdateDashMovement(float DeltaTime) override;
	
	void UpdateGlideMovement(float DeltaTime);

	virtual void DashLocationReached() override;

	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;


};
