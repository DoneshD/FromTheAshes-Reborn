#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraSystemParams.h"
#include "Abilities/GameplayAbility.h"
#include "FTACustomBase/FTAEnums.h"
#include "FTAAbilitySystem/AbilityTypes/FTAAbilityTypes.h"
#include "Player/FTAPlayerState.h"
#include "FTAGameplayAbility.generated.h"

class UCentralStateComponent;
class UComboManagerComponent;
class UFTAAbilityDataAsset;
struct FBaseAbilityDataStruct;
struct FAbilityDataStruct;
class UAbilityTask_MoveToLocation;
class UFTAAT_PlayMontageAndWaitForEvent;
class UAT_WaitInputTagAndQueueWindowEvent;
class UFTAAT_OnTick;
class IFTAAbilitySourceInterface;
class AFTACharacter;
class AFTAPlayerController;
class UPlayerComboManagerComponent;
class USkeletalMeshComponent;

UENUM(BlueprintType)
enum class EFTAAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn
};

UCLASS()
class FROMTHEASHESREBORN_API UFTAGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UFTAAbilitySystemComponent;

private:

	//Activation tags
	UPROPERTY()
	FGameplayTag ActivationIndependentTag = FGameplayTag::RequestGameplayTag("ActivationGroupTag.Independent");

	UPROPERTY()
	FGameplayTag ActivationReplaceableTag = FGameplayTag::RequestGameplayTag("ActivationGroupTag.Exclusive.Replaceable");

	UPROPERTY()
	FGameplayTag ActivationBlockingTag = FGameplayTag::RequestGameplayTag("ActivationGroupTag.Exclusive.Blocking");

	//Tasks
	UPROPERTY()
	TObjectPtr<UFTAAT_OnTick> TickTask;

	UPROPERTY()
	TObjectPtr<UAT_WaitInputTagAndQueueWindowEvent> WaitInputTagAndQueueWindowEventTask;

	UPROPERTY()
	TObjectPtr<UFTAAT_PlayMontageAndWaitForEvent> PlayMontageTask;

protected:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "General", Meta = (Categories = "GameplayAbilityTag"))
	FGameplayTag UniqueIdentifierTag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "General")
	bool NonMontageAbility = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "General")
	bool bEnableTick;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability Activation")
	EFTAAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability Activation")
	FGameplayTag ActivationGroupTag = ActivationBlockingTag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input")
	bool bActivateAbilityOnGranted;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input")
	bool bActivateOnInput;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Ability Assets")
	TArray<TObjectPtr<UFTAAbilityDataAsset>> AbilityAssets;

	UPROPERTY()
	TObjectPtr<UFTAAbilityDataAsset> CurrentAbilityAsset;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	FName WarpTargetName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera")
	FCameraSystemParams CameraParams;

	//Components
	UPROPERTY()
	TObjectPtr<UComboManagerComponent> ComboManagerComponent;

	UPROPERTY()
	TObjectPtr<UCentralStateComponent> CentralStateComponent;
	
	FDelegateHandle AdjustFOVDelegateHandle;

public:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input", Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input", Meta = (Categories = "QueueTag"))
	FGameplayTag QueueWindowTag;
	
public:
	
	UFTAGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION()
	virtual void OnAbilityTick(float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category = "FTA|Ability Getters")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability Getters")
	AFTAPlayerController* GetFTAPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability Getters")
	AFTAPlayerState* GetFTAPlayerStateFromOwnerInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability Getters")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability Getters")
	AFTACharacter* GetFTACharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTAAbility")
	virtual bool IsInputPressed() const;
	
	EFTAAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IFTAAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const;
	
	virtual void PerformAbility(UFTAAbilityDataAsset* InAbilityAsset);
	virtual UFTAAbilityDataAsset* SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets);
	virtual void ExtractAssetProperties(UFTAAbilityDataAsset* InAbilityAsset);
	
	UFUNCTION()
	virtual void SetRuntimeAbilityData(UFTAAbilityDataAsset* InAbilityRuntimeData);
	
	virtual void PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage);

	UFUNCTION()
	virtual void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	virtual void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	virtual void OnMontageBlendingOut(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION(BlueprintCallable)
	virtual void EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void AdjustFOV(const FGameplayTag InEnableTag, int32 NewCount);

	UFUNCTION(BlueprintCallable)
	virtual void ResetAbility();
	
};
