﻿#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GameplayCueManager.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "FTAAbilitySystem/TagRelationships/FTAAbilityTagRelationshipMapping.h"
#include "Player/FTAPlayerController.h"

UFTAAbilitySystemComponent::UFTAAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

	if (InOwnerActor)
	{
		// Check if the owner is a pawn and get its controller
		if (APawn* OwnerPawn = Cast<APawn>(InOwnerActor))
		{
			PlayerController =  Cast<AFTAPlayerController>(OwnerPawn->GetController());
			if(!PlayerController)
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerController is NULL"));
			}
			// PCM = OwnerPawn->FindComponentByClass<UPlayerComboManagerComponent>();

			// if(!PCM)
			// {
			// 	UE_LOG(LogTemp, Error, TEXT("PCM is NULL"));
			// }
		}
	}
	
}

void UFTAAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc)
{
	ABILITYLIST_SCOPE_LOCK();
	for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if(!AbilitySpec.IsActive())
		{
			continue;
		}
		
		//Getting the Class Default Object of GA
		UFTAGameplayAbility* FTAAbilityCDO = CastChecked<UFTAGameplayAbility>(AbilitySpec.Ability);

		//Make sure we do have an instanced ability per actor or execution so we do not operate on the CDO
		if(FTAAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			//Get all instances of the ability (includes per execution)
			TArray<UGameplayAbility*> AllInstances = AbilitySpec.GetAbilityInstances();

			for(UGameplayAbility* AbilityInstance : AllInstances)
			{
				UFTAGameplayAbility* FTAAbilityInstance = CastChecked<UFTAGameplayAbility>(AbilityInstance);

				// ----??-----
				if(ShouldCancelFunc(FTAAbilityInstance, AbilitySpec.Handle))
				{
					if(FTAAbilityInstance->CanBeCanceled())
					{
						/**
						*Cancels the instance of the ability:
						*Note: AbilitySpec.Handle is info shared across all instances, like a common description of it
						* FTAAbilityInstance->GetCurrentActivationInfo() is info from this specific instance
						*/
						FTAAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FTAAbilityInstance->GetCurrentActivationInfo(), false);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."), *FTAAbilityInstance->GetName());
					}
				}
			}
		}
		else
		{
			// Cancel the non-instanced ability CDO.
			if (ShouldCancelFunc(FTAAbilityCDO, AbilitySpec.Handle))
			{
				// Non-instanced abilities can always be canceled.
				check(FTAAbilityCDO->CanBeCanceled());
				FTAAbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), false);
			}
		}
	}
}

void UFTAAbilitySystemComponent::CancelInputActivatedAbilities()
{
	auto ShouldCancelFunc = [this](const UFTAGameplayAbility* FTAAbility, FGameplayAbilitySpec Handle)
	{
		const EFTAAbilityActivationPolicy ActivationPolicy = FTAAbility->GetActivationPolicy();
		return((ActivationPolicy == EFTAAbilityActivationPolicy::OnInputTriggered || ActivationPolicy == EFTAAbilityActivationPolicy::WhileInputActive));
	};
}

void UFTAAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	bool BlockingAbilityActive = ActivationGroupCount[(uint8)EFTAAbilityActivationGroup::Exclusive_Blocking] > 0;
	bool ReplaceableAbilityActive = ActivationGroupCount[(uint8)EFTAAbilityActivationGroup::Exclusive_Replaceable] > 0;
	if(BlockingAbilityActive)
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
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
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
			if(AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
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
		// TryActivateAbility(Spec.Handle);
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UFTAAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UFTAAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// Process all abilities that activate when the input is held.
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

	// Process all abilities that had their input pressed this frame.

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

	//Activate all ability that were pressed or held
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

bool UFTAAbilitySystemComponent::IsActivationGroupBlocked(EFTAAbilityActivationGroup Group) const
{
	bool IsBlocked = false;
	
	switch (Group)
	{
	case EFTAAbilityActivationGroup::Independent:
		IsBlocked = false;
		break;
		
	case EFTAAbilityActivationGroup::Exclusive_Replaceable:
	case EFTAAbilityActivationGroup::Exclusive_Blocking:
		IsBlocked = (ActivationGroupCount[(uint8)EFTAAbilityActivationGroup::Exclusive_Blocking] > 0);
		break;

	default:
		UE_LOG(LogTemp, Error, TEXT("IsActivationGroupBlocked: Invalid Activation Group"));
		break;
	}
	
	return IsBlocked;
}

void UFTAAbilitySystemComponent::AddAbilityToActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* FTAAbility)
{
	if(!FTAAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("AddAbilityToActivationGroup: Invalid Ability to Add"));
		return;
	}
	
	ActivationGroupCount[(uint8)Group]++;
	switch (Group)
	{
	case EFTAAbilityActivationGroup::Independent:
		break;
	case EFTAAbilityActivationGroup::Exclusive_Replaceable:
	case EFTAAbilityActivationGroup::Exclusive_Blocking:
		CancelActivationGroupAbilities(EFTAAbilityActivationGroup::Exclusive_Replaceable, FTAAbility);
		break;
	
	default:
		UE_LOG(LogTemp, Error, TEXT("AddAbilityToActivationGroup: Invalid Group"));
		break;
	}
	const int32 ExclusiveCount = ActivationGroupCount[(uint8)EFTAAbilityActivationGroup::Exclusive_Replaceable] + ActivationGroupCount[(uint8)EFTAAbilityActivationGroup::Exclusive_Blocking];
	if (ExclusiveCount > 1)
	{
		UE_LOG(LogTemp, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running."));
	}
}

void UFTAAbilitySystemComponent::CancelActivationGroupAbilities(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* IgnoreFTAAbility)
{
	auto ShouldCancelFunc = [this, Group, IgnoreFTAAbility](const UFTAGameplayAbility* LyraAbility, FGameplayAbilitySpecHandle Handle)
	{
		return ((LyraAbility->GetActivationGroup() == Group) && (LyraAbility != IgnoreFTAAbility));
	};

	CancelAbilitiesByFunc(ShouldCancelFunc);
}

void UFTAAbilitySystemComponent::RemoveAbilityFromActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* IgnoreFTAAbility)
{
	ActivationGroupCount[(uint8)Group]--;
}

bool UFTAAbilitySystemComponent::CanChangeActivationGroup(EFTAAbilityActivationGroup NewGroup, UFTAGameplayAbility* Ability) const
{
	// if ((NewGroup == EFTAAbilityActivationGroup::Exclusive_Replaceable) && !Ability->CanBeCanceled())
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("CanChangeActivationGroup: This ability can't become replaceable if it can't be canceled."));
	// 	return false;
	// }

	return true;
}

bool UFTAAbilitySystemComponent::ChangeActivationGroup(EFTAAbilityActivationGroup NewGroup, UFTAGameplayAbility* Ability)
{
	if (!CanChangeActivationGroup(NewGroup, Ability))
	{
		return false;
	}

	if(Ability->ActivationGroup != NewGroup)
	{
		RemoveAbilityFromActivationGroup(Ability->ActivationGroup, Ability);
		AddAbilityToActivationGroup(NewGroup, Ability);

		Ability->ActivationGroup = NewGroup;
	}
	return true;
}

void UFTAAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	UFTAGameplayAbility* FTAAbility = CastChecked<UFTAGameplayAbility>(Ability);

	AddAbilityToActivationGroup(FTAAbility->GetActivationGroup(), FTAAbility);
}

void UFTAAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);
}

void UFTAAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	UFTAGameplayAbility* FTAAbility = CastChecked<UFTAGameplayAbility>(Ability);

	RemoveAbilityFromActivationGroup(FTAAbility->GetActivationGroup(), FTAAbility);
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

UFTAAbilitySystemComponent* UFTAAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UFTAAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}

void UFTAAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	// Consume the input if this InputID is overloaded with GenericConfirm/Cancel and the GenericConfim/Cancel callback is bound
	if (IsGenericConfirmInputBound(InputID))
	{
		InputConfirm();
		return;
	}
	
	if (IsGenericCancelInputBound(InputID))
	{
		InputCancel();
		return;
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

