#pragma once

#include "CoreMinimal.h"
#include "../FTAGameplayAbility.h"
#include "GA_Dash.generated.h"

class UFTAAbilityDataAsset;

UCLASS()
class FROMTHEASHESREBORN_API UGA_Dash : public UFTAGameplayAbility
{
	GENERATED_BODY()

private:

	// Dashing
	FVector DashStartLocation;
	FVector DashEndLocation;
	
	float DashElapsedTime = 0.0f;
	float DashStartTime = 0.0f;

	// Gliding
	FVector GlideVelocity;
	bool IsGliding = false;
	
	float GlideElapsedTime = 0.0f;
	float GlideTimeDuration = 1.0f;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	TArray<TObjectPtr<UFTAAbilityDataAsset>> DashAbilityAssets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashDistance = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashDuration = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float ForwardGlideSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float InitialDownwardVelocity = -100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float GlideGravityScale = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
	float GlideDrag = 0.98f;

protected:

	UGA_Dash();

	virtual void OnAbilityTick(float DeltaTime) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

};
