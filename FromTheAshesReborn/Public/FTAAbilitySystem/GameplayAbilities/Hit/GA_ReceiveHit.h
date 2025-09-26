﻿#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GA_ReceiveHit.generated.h"

enum class ESpatialDirection : uint8;
class UHitReactionDataAsset;

UCLASS()
class FROMTHEASHESREBORN_API UGA_ReceiveHit : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:
	
	FGameplayTagContainer HitTagContainer;

public:
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UHitReactionDataAsset>> HitAbilityAssets;

	FGameplayTag ReceiveHitTag = FGameplayTag::EmptyTag;
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Data")
	// TSubclassOf<UGameplayEffect> HitEffect;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Data")
	// FGameplayTag HitTag;

	UPROPERTY(EditDefaultsOnly, Category="Hit")
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> ReceiveHit;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Data")
	FGameplayTag CharacterOrientationTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Data")
	TArray<TSubclassOf<UGA_ReceiveHit>> PossibleHitReactions;

public:

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
