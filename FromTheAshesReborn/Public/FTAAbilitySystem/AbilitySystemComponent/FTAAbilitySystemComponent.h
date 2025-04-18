﻿#pragma once

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

//TODO: DEBUG REMOVE LATER

protected:
	int32 ActivationGroupCount[(uint8)EFTAAbilityActivationGroup::MAX];
	
public:
	TObjectPtr<AWeaponActorBase> TestWeaponActor;

public:

	AFTAPlayerController* PlayerController;
	
	UPROPERTY()
	TObjectPtr<UFTAAbilityTagRelationshipMapping> TagRelationshipMapping;

	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	FWaitQueueInputDelegate OnInputQueueReceived;
	FGameplayTag QueuedInputTag;
	
	FReceivedDamageDelegate ReceivedDamage;
	
public:
	
	UFTAAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	typedef TFunctionRef<bool(const UFTAGameplayAbility* FTAAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc);

	//TODO: Might not be needed
	void CancelInputActivatedAbilities();

	virtual void AbilityLocalInputPressed(int32 InputID) override;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	bool IsActivationGroupBlocked(EFTAAbilityActivationGroup Group) const;
	void AddAbilityToActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* FTAAbility);
	void CancelActivationGroupAbilities(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* IgnoreFTAAbility);
	void RemoveAbilityFromActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* IgnoreFTAAbility);

	UFUNCTION(BlueprintCallable, Category = "FTA|Activation Group")
	bool CanChangeActivationGroup(EFTAAbilityActivationGroup NewGroup, UFTAGameplayAbility* Ability) const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Activation Group")
	bool ChangeActivationGroup(EFTAAbilityActivationGroup NewGroup, UFTAGameplayAbility* Ability);
	
	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;
	
	// Uses a gameplay effect to add the specified dynamic granted tag.
	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);

	// Removes all active instances of the gameplay effect that was used to add the specified dynamic granted tag.
	void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);

	/** Gets the ability target data associated with the given ability handle and activation info */
	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	/** Sets the current tag relationship mapping, if null it will clear it out */
	void SetTagRelationshipMapping(UFTAAbilityTagRelationshipMapping* NewMapping);
	
	/** Looks at ability tags and gathers additional required and blocking tags */
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "AddLooseGameplayTag"))
	void K2_AddLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);
	
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "AddLooseGameplayTags"))
	void K2_AddLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);
	
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "RemoveLooseGameplayTag"))
	void K2_RemoveLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);
	
	UFUNCTION(BlueprintCallable, Meta = (DisplayName = "RemoveLooseGameplayTags"))
	void K2_RemoveLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count = 1);
	
	// Called from FTADamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage.
	virtual void ReceiveDamage(UFTAAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);
	
	// Version of function in AbilitySystemGlobals that returns correct type
	static UFTAAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);

	
	
};