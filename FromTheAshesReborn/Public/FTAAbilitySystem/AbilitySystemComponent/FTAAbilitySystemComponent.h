#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "FTAAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, UFTAAbilitySystemComponent*, SourceASC, float,
                                               UnmitigatedDamage, float, MitigatedDamage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitQueueInputDelegate, FGameplayTag, InputTag);


//DEBUG REMOVE LATER
class AWeaponActorBase;

class USkeletalMeshComponent;
class UFTAAT_PlayMontageAndWaitForEvent;
class UFTAAbilityTagRelationshipMapping;
class UPlayerComboManagerComponent;
class UFTAGameplayAbility;


UCLASS()
class FROMTHEASHESREBORN_API UFTAAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()


public:
	FWaitQueueInputDelegate OnInputQueueReceived;
	FReceivedDamageDelegate ReceivedDamage;
	
protected:

	int32 ActivationGroupCount[static_cast<uint8>(EFTAAbilityActivationGroup::MAX)];
	
	TObjectPtr<UFTAAbilityTagRelationshipMapping> TagRelationshipMapping;
	
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	UPROPERTY(BlueprintReadOnly, Category = "Queue")
	FGameplayTag QueuedInputTag;
	
public:
	
	UFTAAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	static UFTAAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);

	virtual void AbilityLocalInputPressed(int32 InputID) override;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	typedef TFunctionRef<bool(const UFTAGameplayAbility* FTAAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc);

	void CancelInputActivatedAbilities();

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	bool IsActivationGroupBlocked(EFTAAbilityActivationGroup Group) const;
	void AddAbilityToActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* FTAAbility);
	void CancelActivationGroupAbilities(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* IgnoreFTAAbility);
	void RemoveAbilityFromActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* IgnoreFTAAbility);

	bool CanChangeActivationGroup(EFTAAbilityActivationGroup NewGroup, UFTAGameplayAbility* Ability) const;
	bool ChangeActivationGroup(EFTAAbilityActivationGroup NewGroup, UFTAGameplayAbility* Ability);

	bool IsAbilityActive(TSubclassOf<UGameplayAbility> AbilityClass) const;
	void CancelAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass);
	
	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;
	
	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);
	void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);

	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	void SetTagRelationshipMapping(UFTAAbilityTagRelationshipMapping* NewMapping);
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	UFUNCTION(BlueprintCallable, Category = "GameplayTags", Meta = (DisplayName = "AddLooseGameplayTag"))
	void K2_AddLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", Meta = (DisplayName = "AddLooseGameplayTags"))
	void K2_AddLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", Meta = (DisplayName = "RemoveLooseGameplayTag"))
	void K2_RemoveLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayTags", Meta = (DisplayName = "RemoveLooseGameplayTags"))
	void K2_RemoveLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);
	
	virtual void ReceiveDamage(UFTAAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);
	
};