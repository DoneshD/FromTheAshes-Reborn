#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Animation/AnimInstance.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "GameplayCueManager.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"

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
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
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
	if (InputTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Tag Pressed: %s"), *InputTag.ToString());

		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability)
			{
				UE_LOG(LogTemp, Warning, TEXT("Checking Ability: %s"), *AbilitySpec.Ability->GetClass()->GetName());

				if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
				{
					UE_LOG(LogTemp, Warning, TEXT("Matched Ability: %s"), *AbilitySpec.Ability->GetClass()->GetName());

					InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
					InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
				}
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

void UFTAAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	if(HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Gameplay.AbilityInputBlocked")))
	{
		ClearAbilityInput();
		return;
	}
	
	// Clear all abilities in array not being pressed
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// Process all abilities that activate when the input is held.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if(const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability && AbilitySpec->IsActive())
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
					//Ability is already active, passing along event
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
					// Ability is active so pass along the input event.
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

	//Independent abilities cant be blocked
	case EFTAAbilityActivationGroup::Independent:
		IsBlocked = false;
		break;

	case EFTAAbilityActivationGroup::Exclusive_Replaceable:
	case EFTAAbilityActivationGroup::Exclusive_Blocking:
		IsBlocked = (ActivationGroupCounts[(uint8)EFTAAbilityActivationGroup::Exclusive_Blocking] > 0);
		break;

	default:
		checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}
	return IsBlocked;
}

void UFTAAbilitySystemComponent::AddAbilityToActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* FTAAbility)
{
	check(FTAAbility);
	check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);

	ActivationGroupCounts[(uint8)Group]++;

	switch (Group)
	{
	case EFTAAbilityActivationGroup::Independent:
		// Independent abilities do not cancel any other abilities.
			break;

	case EFTAAbilityActivationGroup::Exclusive_Replaceable:
	case EFTAAbilityActivationGroup::Exclusive_Blocking:
		CancelActivationGroupAbilities(EFTAAbilityActivationGroup::Exclusive_Replaceable, FTAAbility);
		break;

	default:
		checkf(false, TEXT("AddAbilityToActivationGroup: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	const int32 ExclusiveCount = ActivationGroupCounts[(uint8)EFTAAbilityActivationGroup::Exclusive_Replaceable] + ActivationGroupCounts[(uint8)EFTAAbilityActivationGroup::Exclusive_Blocking];
	if (!ensure(ExclusiveCount <= 1))
	{
		UE_LOG(LogTemp, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running."));
	}
}

void UFTAAbilitySystemComponent::RemoveAbilityFromActivationGroup(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* FTAAbility)
{
	check(FTAAbility)
	check(ActivationGroupCounts[(uint8)Group] > 0);

	ActivationGroupCounts[(uint8)Group]--;
}

void UFTAAbilitySystemComponent::CancelActivationGroupAbilities(EFTAAbilityActivationGroup Group, UFTAGameplayAbility* IgnoreFTAAbility)
{
	auto ShouldCancelFunc = [this, Group, IgnoreFTAAbility](const UFTAGameplayAbility* FTAAbility, FGameplayAbilitySpecHandle Handle)
	{
		return ((FTAAbility->GetActivationGroup() == Group) && (FTAAbility != IgnoreFTAAbility));
	};

	CancelAbilitiesByFunc(ShouldCancelFunc);
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
	
}

void UFTAAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	
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

	// ---------------------------------------------------------

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID)
		{
			if (Spec.Ability)
			{
				Spec.InputPressed = true;
				
				if (Spec.IsActive())
				{
					if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
					{
						ServerSetInputPressed(Spec.Handle);
					}
					AbilitySpecInputPressed(Spec);

					// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
				}
				else
				{
					UFTAGameplayAbility* GA = Cast<UFTAGameplayAbility>(Spec.Ability);
					if (GA && GA->bActivateOnInput)
					{
						// Ability is not active, so try to activate it
						TryActivateAbility(Spec.Handle);
					}
				}
				
			}
		}
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

float UFTAAbilitySystemComponent::PlayMontageForMesh(UGameplayAbility* InAnimatingAbility, USkeletalMeshComponent* InMesh, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName, bool bReplicateMontage)
{
	UFTAGameplayAbility* InAbility = Cast<UFTAGameplayAbility>(InAnimatingAbility);

	float Duration = -1.f;

	UAnimInstance* AnimInstance = IsValid(InMesh) && InMesh->GetOwner() == AbilityActorInfo->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	if (AnimInstance && NewAnimMontage)
	{
		Duration = AnimInstance->Montage_Play(NewAnimMontage, InPlayRate);
		if (Duration > 0.f)
		{
			if (NewAnimMontage->HasRootMotion() && AnimInstance->GetOwningActor())
			{
				UE_LOG(LogRootMotion, Log, TEXT("UAbilitySystemComponent::PlayMontage %s, Role: %s")
					, *GetNameSafe(NewAnimMontage)
					, *UEnum::GetValueAsString(TEXT("Engine.ENetRole"), AnimInstance->GetOwningActor()->GetLocalRole())
					);
			}
			
			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, NewAnimMontage);
			}

		}
	}
	return Duration;
}

void UFTAAbilitySystemComponent::ReceiveDamage(UFTAAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}

