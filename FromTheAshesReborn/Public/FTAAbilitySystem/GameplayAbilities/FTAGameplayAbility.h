﻿#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FTACustomBase/FTAEnums.h"
#include "FTAAbilitySystem/AbilityTypes/FTAAbilityTypes.h"
#include "FTAGameplayAbility.generated.h"

class IFTAAbilitySourceInterface;
class AFTACharacter;
class AFTAPlayerController;
class UPlayerComboManagerComponent;
class USkeletalMeshComponent;


UENUM(BlueprintType)
enum class EFTAAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

UENUM(BlueprintType)
enum class EFTAAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,
	
	//MAX	UMETA(Hidden)
	MAX	UMETA(Hidden)
};

UCLASS()
class FROMTHEASHESREBORN_API UFTAGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UFTAAbilitySystemComponent;

//variables
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA|Ability Activation")
	EFTAAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FTA|Ability Activation")
	EFTAAbilityActivationGroup ActivationGroup;


//functions
public:
	UFTAGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability")
	AFTAPlayerController* GetFTAPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "FTA|Ability")
	AFTACharacter* GetFTACharacterFromActorInfo() const;

	EFTAAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	EFTAAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool IsInputPressed() const;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IFTAAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const;

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input")
	EAbilityInputID AbilityInputID = EAbilityInputID::None;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	bool bActivateAbilityOnGranted;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	bool bActivateOnInput;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tags")
	FGameplayTag UniqueIdentifierTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combo")
	bool CanBeCanceledForQueue = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combo")
	FGameplayTag ComboWindowTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combo")
	FGameplayTagContainer QueueableAbilitiesTags;
	
};
