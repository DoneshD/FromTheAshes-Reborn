#pragma once

#include "CoreMinimal.h"
#include "GA_Attack.h"
#include "GA_RangedAttack.generated.h"

class UTargetingSystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API UGA_RangedAttack : public UGA_Attack
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<UTargetingSystemComponent> TargetingSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	TSubclassOf<AActor> TargetableActors;

	UPROPERTY()
	AActor* TargetActor;

protected:

	UGA_RangedAttack(const FObjectInitializer& = FObjectInitializer::Get());
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void RangedTargetFound(TObjectPtr<AActor> Target); 

	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage) override;
	
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData) override;
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData) override;

	virtual FGameplayAbilityTargetDataHandle AddHitResultToTargetData(const FHitResult& LastItem) override;
	virtual void ExecuteHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle) override;

	virtual UFTAAbilityDataAsset* SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets) override;
	virtual void ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset) override;
	virtual void PerformAbility(UFTAAbilityDataAsset* InAbilityAsset) override;

	TArray<AActor*> GetAllActorsOfClass(TSubclassOf<AActor> ActorClass) const;
	AActor* FindNearestTargetToActor(TArray<AActor*> Actors) const;

	virtual void OnHitAdded(FHitResult LastItem) override;
};
