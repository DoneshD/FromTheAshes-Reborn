#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GA_ReceiveHit.generated.h"

class UHitReactionDataAsset;

UCLASS()
class FROMTHEASHESREBORN_API UGA_ReceiveHit : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer HitAbilityTagContainer;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UHitReactionDataAsset>> HitAbilityAssets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE", Meta = (Categories = "HitTag"))
	FGameplayTag HitReactionTag;

public:

	bool bCanceled = false;

	UGA_ReceiveHit();

	virtual void OnAbilityTick(float DeltaTime) override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;
	
};
