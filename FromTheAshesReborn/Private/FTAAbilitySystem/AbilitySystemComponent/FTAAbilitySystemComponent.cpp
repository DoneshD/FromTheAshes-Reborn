#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GameplayCueManager.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "FTAAbilitySystem/TagRelationships/FTAAbilityTagRelationshipMapping.h"

UFTAAbilitySystemComponent::UFTAAbilitySystemComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void UFTAAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UFTAAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);
	
}

UFTAAbilitySystemComponent* UFTAAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UFTAAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}

void UFTAAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	if (IsGenericConfirmInputBound(InputID))
	{
		InputConfirm();
	}
	
	if (IsGenericCancelInputBound(InputID))
	{
		InputCancel();
	}
}

void UFTAAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	const bool BlockingAbilityActive = CurrentlyActiveAbilityOfActivationGroup(ActivationBlockingTag);
	const bool ReplaceableAbilityActive = CurrentlyActiveAbilityOfActivationGroup(ActivationReplaceableTag);
	
	if(BlockingAbilityActive || ReplaceableAbilityActive)
	{
		if (InputTag.IsValid() && !InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("InputTag.Hold.Ability.LockOn")) && HasMatchingGameplayTag(InputQueueOpen))
		// if (InputTag.IsValid() && !InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("InputTag.Hold.Ability.LockOn")))
		{
			
			OnInputQueueReceived.Broadcast(InputTag);
			return;
		}
	}
	
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
	
}

void UFTAAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if(AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UFTAAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
	
	if (Spec.IsActive())
	{
		
	}
}

void UFTAAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		// CancelAbilityHandle(Spec.Handle);
	}
}

void UFTAAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if(const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				const UFTAGameplayAbility* FTAAbilityCDO = CastChecked<UFTAGameplayAbility>(AbilitySpec->Ability);
				if(FTAAbilityCDO->GetActivationPolicy() == EFTAAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}
	
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if(AbilitySpec->IsActive())
				{
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UFTAGameplayAbility* FTAAbilityCDO = CastChecked<UFTAGameplayAbility>(AbilitySpec->Ability);
					if(FTAAbilityCDO->GetActivationPolicy() == EFTAAbilityActivationPolicy::OnInputTriggered)
					{
						
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	for(const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;
				if(AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}
	
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UFTAAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UFTAAbilitySystemComponent::CancelInputActivatedAbilities()
{
	auto ShouldCancelFunc = [this](const UFTAGameplayAbility* FTAAbility, FGameplayAbilitySpec Handle)
	{
		const EFTAAbilityActivationPolicy ActivationPolicy = FTAAbility->GetActivationPolicy();
		return((ActivationPolicy == EFTAAbilityActivationPolicy::OnInputTriggered || ActivationPolicy == EFTAAbilityActivationPolicy::WhileInputActive));
	};
}

void UFTAAbilitySystemComponent::CancelAbilitiesWithRuntimeTag(const FGameplayTag& TagToCancel)
{
	TArray<UGameplayAbility*> AbilitiesToCancel;

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.IsActive())
		{
			if (Spec.GetDynamicSpecSourceTags().HasTag(TagToCancel))
			{
				if (Spec.Ability)
				{
					AbilitiesToCancel.Add(Spec.Ability);
				}
			}
		}
	}

	for (UGameplayAbility* Ability : AbilitiesToCancel)
	{
		CancelAbility(Ability);
	}

	UE_LOG(LogTemp, Log, TEXT("Cancelled %d abilities with runtime tag [%s]."), AbilitiesToCancel.Num(), *TagToCancel.ToString());
}


bool UFTAAbilitySystemComponent::CurrentlyActiveAbilityOfActivationGroup(FGameplayTag GroupToCheck)
{
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.ActiveCount > 0 && Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTagExact(GroupToCheck))
		{
			return true;
		}
	}

	return false;
}

bool UFTAAbilitySystemComponent::IsActivationGroupTagBlocked(FGameplayTag GroupToCheck)
{
	if(GroupToCheck == ActivationIndependentTag)
	{
		return false;
	}
	else if(GroupToCheck == ActivationReplaceableTag)
	{
		return CurrentlyActiveAbilityOfActivationGroup(ActivationReplaceableTag);
	}
	else if(GroupToCheck == ActivationBlockingTag)
	{
		return CurrentlyActiveAbilityOfActivationGroup(ActivationBlockingTag);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UFTAAbilitySystemComponent::IsActivationGroupTagBlocked - No matching tag"))
	}
	return false;
}

void UFTAAbilitySystemComponent::AddToActivationGroup(FGameplayTag Group, UFTAGameplayAbility* FTAAbility)
{
	if (!FTAAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("RemoveFromActivationGroup failed: Ability is null."));
		return;
	}

	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(FTAAbility->GetClass());
	if (!Spec)
	{
		// UE_LOG(LogTemp, Error, TEXT("RemoveFromActivationGroup failed: Spec not found for ability [%s]."), *FTAAbility->GetName());
		return;
	}

	Spec->GetDynamicSpecSourceTags().AddTag(Group);
	
}

UFTAGameplayAbility* UFTAAbilitySystemComponent::GetCurrentlyActiveExclusiveAbility()
{
	
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.ActiveCount > 0)
		{
			if (UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(Spec.Ability))
			{
				if (FTAAbility->GetCurrentAbilitySpec()->GetDynamicSpecSourceTags().HasTagExact(ActivationBlockingTag))
				{
					UE_LOG(LogTemp, Warning, TEXT("Active Exclusive Ability: %s"), *FTAAbility->GetName());
					return FTAAbility;
				}
			}
		}
	}
	
	return nullptr;
}

void UFTAAbilitySystemComponent::RemoveFromActivationGroup(FGameplayTag Group, UFTAGameplayAbility* FTAAbility)
{
	if (!FTAAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("RemoveFromActivationGroup failed: Ability is null."));
		return;
	}
	
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(FTAAbility->GetClass());
	if (!Spec)
	{
		// UE_LOG(LogTemp, Error, TEXT("RemoveFromActivationGroup failed: Spec not found for ability [%s]."), *FTAAbility->GetName());
		return;
	}

	Spec->GetDynamicSpecSourceTags().RemoveTag(Group);
}


bool UFTAAbilitySystemComponent::ChangeToActivationGroup(FGameplayTag NewGroup, UFTAGameplayAbility* Ability)
{
	if (!Ability)
	{
		UE_LOG(LogTemp, Error, TEXT("ChangeToActivationGroup failed: Ability is null."));
		return false;
	}

	if (Ability->ActivationGroupTag != NewGroup)
	{
		RemoveFromActivationGroup(Ability->ActivationGroupTag, Ability);
		AddToActivationGroup(NewGroup, Ability);

		Ability->ActivationGroupTag = NewGroup;
	}
	return true;
}

bool UFTAAbilitySystemComponent::IsAbilityActive(TSubclassOf<UGameplayAbility> AbilityClass) const
{
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->GetClass() == AbilityClass && Spec.ActiveCount > 0)
		{
			return true;
		}
	}
	return false;
}

void UFTAAbilitySystemComponent::CancelAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->GetClass() == AbilityClass)
		{
			CancelAbilityHandle(Spec.Handle);
			return;
		}
	}
}

void UFTAAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	UFTAGameplayAbility* FTAAbility = CastChecked<UFTAGameplayAbility>(Ability);
}

void UFTAAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);
}

void UFTAAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	UFTAGameplayAbility* FTAAbility = CastChecked<UFTAGameplayAbility>(Ability);
	
}

void UFTAAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags,
	const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
	}
	
	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, ModifiedBlockTags, bExecuteCancelTags, ModifiedCancelTags);
}


void UFTAAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
{
	Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);
}


void UFTAAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	
}

void UFTAAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	
}

void UFTAAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	
}

void UFTAAbilitySystemComponent::SetTagRelationshipMapping(UFTAAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

void UFTAAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void UFTAAbilitySystemComponent::K2_AddLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count)
{
	AddLooseGameplayTag(GameplayTag, Count);
}
void UFTAAbilitySystemComponent::K2_AddLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	AddLooseGameplayTags(GameplayTags, Count);
}
void UFTAAbilitySystemComponent::K2_RemoveLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count)
{
	RemoveLooseGameplayTag(GameplayTag, Count);
}
void UFTAAbilitySystemComponent::K2_RemoveLooseGameplayTags(const FGameplayTagContainer& GameplayTags, int32 Count)
{
	RemoveLooseGameplayTags(GameplayTags, Count);
}

void UFTAAbilitySystemComponent::ReceiveDamage(UFTAAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}

void UFTAAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::WhileActive, GameplayCueParameters);
}

void UFTAAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);
}
