#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GameplayCueManager.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "InterchangeResult.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
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
	// for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	// {
	// 	if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
	// 	{
	// 		UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(AbilitySpec.Ability);
	// 		if(FTAAbility)
	// 		{
	// 			// if (IsAbilityActive(AbilitySpec.Ability->GetClass()) && FTAAbility->DefaultActivationGroup == EFTAAbilityActivationGroup::Exclusive_Replaceable)
	// 			// {
	// 			// 	CancelAbilityByClass(AbilitySpec.Ability->GetClass());
	// 			// }
	// 			FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(FTAAbility->GetClass());
	// 			
	// 			if (!Spec)
	// 			{
	// 				UE_LOG(LogTemp, Error, TEXT("RemoveFromActivationGroup failed: Spec not found for ability [%s]."), *FTAAbility->GetName());
	// 				return;
	// 			}
	//
	// 			const FGameplayTagContainer& DynamicTags = Spec->GetDynamicSpecSourceTags();
	//
	// 			UE_LOG(LogTemp, Warning, TEXT("DynamicSpecSourceTags for ability [%s]:"), *Spec->Ability->GetName());
	//
	// 			for (const FGameplayTag& Tag : DynamicTags)
	// 			{
	// 				UE_LOG(LogTemp, Error, TEXT("- %s"), *Tag.ToString());
	// 			}
	//
	//
	// 			if (IsAbilityActive(Spec->Ability->GetClass()) && Spec->GetDynamicSpecSourceTags().HasTagExact(ActivationReplaceableTag))
	// 			{
	// 				CancelAbilityByClass(Spec->Ability->GetClass());
	// 			}
	// 			
	// 		}
	// 	}
	// }

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if(Spec.Ability && (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
		{
			const FGameplayTagContainer& DynamicTags = Spec.GetDynamicSpecSourceTags();
			if (IsAbilityActive(Spec.Ability->GetClass()) && Spec.GetDynamicSpecSourceTags().HasTagExact(ActivationReplaceableTag))
			{
				CancelAbilityByClass(Spec.Ability->GetClass());
			}
			UE_LOG(LogTemp, Log, TEXT("Dynamic source tags for ability [%s] after group change:"), *Spec.Ability->GetName());
			for (const FGameplayTag& Tag : DynamicTags)
			{
				UE_LOG(LogTemp, Log, TEXT("- %s"), *Tag.ToString());
			}
			break;
		}
	}

	// const bool BlockingAbilityActive = ActivationGroupCount[static_cast<uint8>(EFTAAbilityActivationGroup::Exclusive_Blocking)] > 0;
	// const bool ReplaceableAbilityActive = ActivationGroupCount[static_cast<uint8>(EFTAAbilityActivationGroup::Exclusive_Replaceable)] > 0;

	const bool BlockingAbilityActive = CurrentlyActiveAbilityOfActivationGroup(ActivationBlockingTag);
	const bool ReplaceableAbilityActive = CurrentlyActiveAbilityOfActivationGroup(ActivationReplaceableTag);
	
	if(BlockingAbilityActive || ReplaceableAbilityActive)
	{
		if (InputTag.IsValid())
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

// void UFTAAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc)
// {
// 	ABILITYLIST_SCOPE_LOCK();
// 	for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
// 	{
// 		if(!AbilitySpec.IsActive())
// 		{
// 			continue;
// 		}
// 		
// 		UFTAGameplayAbility* FTAAbilityCDO = CastChecked<UFTAGameplayAbility>(AbilitySpec.Ability);
//
// 		if(FTAAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
// 		{
// 			TArray<UGameplayAbility*> AllInstances = AbilitySpec.GetAbilityInstances();
//
// 			for(UGameplayAbility* AbilityInstance : AllInstances)
// 			{
// 				UFTAGameplayAbility* FTAAbilityInstance = CastChecked<UFTAGameplayAbility>(AbilityInstance);
//
// 				if(ShouldCancelFunc(FTAAbilityInstance, AbilitySpec.Handle))
// 				{
// 					if(FTAAbilityInstance->CanBeCanceled())
// 					{
// 						
// 						FTAAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FTAAbilityInstance->GetCurrentActivationInfo(), false);
// 					}
// 					else
// 					{
// 						UE_LOG(LogTemp, Error, TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."), *FTAAbilityInstance->GetName());
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			if (ShouldCancelFunc(FTAAbilityCDO, AbilitySpec.Handle))
// 			{
// 				check(FTAAbilityCDO->CanBeCanceled());
// 				FTAAbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), false);
// 			}
// 		}
// 	}
// }

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
			UE_LOG(LogTemp, Warning, TEXT("UFTAAbilitySystemComponent::CurrentlyActiveAbilityOfActivationGroup - Found Active Ability: %s"), *Spec.Ability->GetName());
			return true;
		}
	}

	// UE_LOG(LogTemp, Warning, TEXT("UFTAAbilitySystemComponent::CurrentlyActiveAbilityOfActivationGroup - False"))
	return false;
}

// bool UFTAAbilitySystemComponent::IsActivationGroupBlocked(EFTAAbilityActivationGroup Group) const
// {
// 	bool IsBlocked = false;
// 	
// 	switch (Group)
// 	{
// 	case EFTAAbilityActivationGroup::Independent:
// 		IsBlocked = false;
// 		break;
// 		
// 	case EFTAAbilityActivationGroup::Exclusive_Replaceable:
// 	case EFTAAbilityActivationGroup::Exclusive_Blocking:
// 		IsBlocked = (ActivationGroupCount[(uint8)EFTAAbilityActivationGroup::Exclusive_Blocking] > 0);
// 		break;
//
// 	default:
// 		UE_LOG(LogTemp, Error, TEXT("IsActivationGroupBlocked: Invalid Activation Group"));
// 		break;
// 	}
// 	
// 	return IsBlocked;
// }

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

// void UFTAAbilitySystemComponent::AddAbilityToActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* FTAAbility)
// {
// 	if(!FTAAbility)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("AddAbilityToActivationGroup: Invalid Ability to Add"));
// 		return;
// 	}
// 	
// 	ActivationGroupCount[(uint8)Group]++;
// 	switch (Group)
// 	{
// 	case EFTAAbilityActivationGroup::Independent:
// 		break;
// 	case EFTAAbilityActivationGroup::Exclusive_Replaceable:
// 	case EFTAAbilityActivationGroup::Exclusive_Blocking:
// 		CancelActivationGroupAbilities(EFTAAbilityActivationGroup::Exclusive_Replaceable, FTAAbility);
// 		break;
// 	
// 	default:
// 		UE_LOG(LogTemp, Error, TEXT("AddAbilityToActivationGroup: Invalid Group"));
// 		break;
// 	}
// 	const int32 ExclusiveCount = ActivationGroupCount[(uint8)EFTAAbilityActivationGroup::Exclusive_Blocking];
// 	if (ExclusiveCount > 1)
// 	{
// 		CancelAllAbilities();
// 		UE_LOG(LogTemp, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running. Canceling all abilities"));
// 	}
// }

void UFTAAbilitySystemComponent::AddToActivationGroup(FGameplayTag Group, UFTAGameplayAbility* FTAAbility)
{

	if (!FTAAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("RemoveFromActivationGroup failed: Ability is null."));
		return;
	}

	// Get the ability spec from the ability pointer
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(FTAAbility->GetClass());
	if (!Spec)
	{
		UE_LOG(LogTemp, Error, TEXT("RemoveFromActivationGroup failed: Spec not found for ability [%s]."), *FTAAbility->GetName());
		return;
	}

	// Remove the tag from the dynamic source tags
	Spec->GetDynamicSpecSourceTags().AddTag(Group);
	
	UE_LOG(LogTemp, Log, TEXT("Added tag [%s] from dynamic source tags of ability [%s]."),
			*Group.ToString(), *FTAAbility->GetName());
	
	
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

// void UFTAAbilitySystemComponent::CancelActivationGroupAbilities(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* IgnoreFTAAbility)
// {
// 	auto ShouldCancelFunc = [this, Group, IgnoreFTAAbility](const UFTAGameplayAbility* LyraAbility, FGameplayAbilitySpecHandle Handle)
// 	{
// 		return ((LyraAbility->GetActivationGroup() == Group) && (LyraAbility != IgnoreFTAAbility));
// 	};
//
// 	CancelAbilitiesByFunc(ShouldCancelFunc);
// }

// void UFTAAbilitySystemComponent::RemoveAbilityFromActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* IgnoreFTAAbility)
// {
// 	ActivationGroupCount[(uint8)Group]--;
// }

void UFTAAbilitySystemComponent::RemoveFromActivationGroup(FGameplayTag Group, UFTAGameplayAbility* FTAAbility)
{
	if (!FTAAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("RemoveFromActivationGroup failed: Ability is null."));
		return;
	}

	// Get the ability spec from the ability pointer
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(FTAAbility->GetClass());
	if (!Spec)
	{
		UE_LOG(LogTemp, Error, TEXT("RemoveFromActivationGroup failed: Spec not found for ability [%s]."), *FTAAbility->GetName());
		return;
	}

	// Remove the tag from the dynamic source tags
	if (Spec->GetDynamicSpecSourceTags().RemoveTag(Group))
	{
		UE_LOG(LogTemp, Log, TEXT("Removed tag [%s] from dynamic source tags of ability [%s]."),
			*Group.ToString(), *FTAAbility->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Tag [%s] not found in dynamic source tags of ability [%s]."),
			*Group.ToString(), *FTAAbility->GetName());
	}
}


// bool UFTAAbilitySystemComponent::CanChangeActivationGroup(EFTAAbilityActivationGroup NewGroup, UFTAGameplayAbility* Ability) const
// {
// 	if ((NewGroup == EFTAAbilityActivationGroup::Exclusive_Replaceable) && !Ability->CanBeCanceled())
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("CanChangeActivationGroup: This ability can't become replaceable if it can't be canceled."));
// 		return false;
// 	}
//
// 	return true;
// }

// bool UFTAAbilitySystemComponent::ChangeActivationGroup(EFTAAbilityActivationGroup NewGroup, UFTAGameplayAbility* Ability)
// {
// 	if (!CanChangeActivationGroup(NewGroup, Ability))
// 	{
// 		return false;
// 	}
//
// 	if(Ability->ActivationGroup != NewGroup)
// 	{
// 		RemoveAbilityFromActivationGroup(Ability->ActivationGroup, Ability);
// 		AddAbilityToActivationGroup(NewGroup, Ability);
//
// 		Ability->ActivationGroup = NewGroup;
// 	}
// 	return true;
// }

bool UFTAAbilitySystemComponent::ChangeToActivationGroup(FGameplayTag NewGroup, UFTAGameplayAbility* Ability)
{
	if (!Ability)
	{
		UE_LOG(LogTemp, Error, TEXT("ChangeToActivationGroup failed: Ability is null."));
		return false;
	}

	if (Ability->ActivationGroupTag != NewGroup)
	{
		UE_LOG(LogTemp, Warning, TEXT("Changing activation group for ability [%s] from [%s] to [%s]"),
			*Ability->GetName(),
			*Ability->ActivationGroupTag.ToString(),
			*NewGroup.ToString());

		RemoveFromActivationGroup(Ability->ActivationGroupTag, Ability);
		AddToActivationGroup(NewGroup, Ability);

		Ability->ActivationGroupTag = NewGroup;

		// Find the spec associated with this ability and print its dynamic source tags
		for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
		{
			if (Spec.Ability == Ability)
			{
				const FGameplayTagContainer& DynamicTags = Spec.GetDynamicSpecSourceTags();
				
				UE_LOG(LogTemp, Log, TEXT("Dynamic source tags for ability [%s] after group change:"), *Ability->GetName());
				for (const FGameplayTag& Tag : DynamicTags)
				{
					UE_LOG(LogTemp, Log, TEXT("- %s"), *Tag.ToString());
				}
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Ability [%s] is already in activation group [%s], no change needed."),
			*Ability->GetName(),
			*NewGroup.ToString());
	}

	return true;
}



bool UFTAAbilitySystemComponent::IsAbilityActive(TSubclassOf<UGameplayAbility> AbilityClass) const
{
	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->GetClass() == AbilityClass)
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

	// AddToActivationGroup(FTAAbility->ActivationGroupTag, FTAAbility);
}

void UFTAAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);
}

void UFTAAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	UFTAGameplayAbility* FTAAbility = CastChecked<UFTAGameplayAbility>(Ability);
	
	// RemoveFromActivationGroup(FTAAbility->ActivationGroupTag, FTAAbility);
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
	UE_LOG(LogTemp, Warning, TEXT("AddGameplayCueLocal"))
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::WhileActive, GameplayCueParameters);
}

void UFTAAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag,
	const FGameplayCueParameters& GameplayCueParameters)
{
	UE_LOG(LogTemp, Warning, TEXT("RemoveGameplayCueLocal"))
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetOwner(), GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);
}
