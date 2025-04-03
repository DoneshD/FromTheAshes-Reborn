#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FTAAbilitySystem/AbilityTypes/FTATargetType.h"
#include "GameplayTagContainer.h"
#include "FTAAbilitySystem/FTAAbilitySourceInterface.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/AT_WaitQueueInput.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_OnTick.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"
#include "FTACustomBase/FTACharacter.h"
#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"


UFTAGameplayAbility::UFTAGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bActivateAbilityOnGranted = false;
	bActivateOnInput = true;

	ActivationPolicy = EFTAAbilityActivationPolicy::OnInputTriggered;
	
}

void UFTAGameplayAbility::OnAbilityTick(float DeltaTime)
{
	
}

UFTAAbilitySystemComponent* UFTAGameplayAbility::GetFTAAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UFTAAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AFTAPlayerController* UFTAGameplayAbility::GetFTAPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AFTAPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AFTAPlayerState* UFTAGameplayAbility::GetFTAPlayerStateFromOwnerInfo() const
{
	return (CurrentActorInfo ? Cast<AFTAPlayerState>(CurrentActorInfo->OwnerActor.Get()) : nullptr);
}

AController* UFTAGameplayAbility::GetControllerFromActorInfo() const
{
	if(!CurrentActorInfo)
	{
		UE_LOG(LogTemp, Error, TEXT("UFTAGameplayAbility::CurrentActorInfo is null"));
		return nullptr;
	}

	if(AController* PC = CurrentActorInfo->PlayerController.Get())
	{
		return PC;
	}
	
	// Look for a player controller or pawn in the owner chain.
	AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
	while (TestActor)
	{
		if (AController* Controller = Cast<AController>(TestActor))
		{
			return Controller;
		}

		if (APawn* Pawn = Cast<APawn>(TestActor))
		{
			return Pawn->GetController();
		}

		TestActor = TestActor->GetOwner();
	}
	return nullptr;
}

AFTACharacter* UFTAGameplayAbility::GetFTACharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AFTACharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

bool UFTAGameplayAbility::CanChangeActivationGroup(EFTAAbilityActivationGroup NewGroup) const
{
	if (!IsInstantiated())
	{
		UE_LOG(LogTemp, Error, TEXT("Ability is not instantiated"));
		return false;
	}
	
	if (!IsActive())
	{
		UE_LOG(LogTemp, Error, TEXT("Ability is not active"));
		return false;
	}
	
	UFTAAbilitySystemComponent* FTAASC = GetFTAAbilitySystemComponentFromActorInfo();
	if (!FTAASC)
	{
		UE_LOG(LogTemp, Error, TEXT("CanChangeActivationGroup: FTAASC is not valid"));
		return false;
	}
	
	if((ActivationGroup != EFTAAbilityActivationGroup::Exclusive_Blocking) && FTAASC->IsActivationGroupBlocked(NewGroup))
	{
		UE_LOG(LogTemp, Error, TEXT("CanChangeActivationGroup: This ability can't change groups if it's blocked (unless it is the one doing the blocking)."));
		return false;
	}
	
	if ((NewGroup == EFTAAbilityActivationGroup::Exclusive_Replaceable) && !CanBeCanceled())
	{
		UE_LOG(LogTemp, Error, TEXT("CanChangeActivationGroup: This ability can't become replaceable if it can't be canceled."));
		return false;
	}

	return true;
	
}

bool UFTAGameplayAbility::ChangeActivationGroup(EFTAAbilityActivationGroup NewGroup)
{
	if (!IsInstantiated())
	{
		UE_LOG(LogTemp, Error, TEXT("Ability is not instantiated"));
		return false;
	}
	
	if (!IsActive())
	{
		UE_LOG(LogTemp, Error, TEXT("Ability is not active"));
		return false;
	}
	
	UFTAAbilitySystemComponent* FTAASC = GetFTAAbilitySystemComponentFromActorInfo();
	if (!FTAASC)
	{
		UE_LOG(LogTemp, Error, TEXT("CanChangeActivationGroup: FTAASC is not valid"));
		return false;
	}

	if (!CanChangeActivationGroup(NewGroup))
	{
		return false;
	}

	if(ActivationGroup != NewGroup)
	{
		FTAASC->RemoveAbilityFromActivationGroup(ActivationGroup, this);
		FTAASC->AddAbilityToActivationGroup(NewGroup, this);

		ActivationGroup = NewGroup;
	}
	return true;
}

void UFTAGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && ActivationPolicy == EFTAAbilityActivationPolicy::OnSpawn)
	{
		//Might cause problems if ASC is already a raw pointer
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		if(!ASC)
		{
			UE_LOG(LogTemp, Error, TEXT("UFTAGameplayAbility::ASC is null"));
			return;
		}

		if(!AvatarActor)
		{
			UE_LOG(LogTemp, Error, TEXT("UFTAGameplayAbility::AvatarActor is null"));
			return;
		}

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor)
		{
			ASC->TryActivateAbility(Spec.Handle);
		}
	}
}

void UFTAGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

bool UFTAGameplayAbility::IsInputPressed() const
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	return Spec && Spec->InputPressed;
}

void UFTAGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

bool UFTAGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}
	
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UFTAAbilitySystemComponent* FTAASC = CastChecked<UFTAAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	if(FTAASC->IsActivationGroupBlocked(ActivationGroup))
	{
		return false;
	}

	return true;
	
}

void UFTAGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
	Super::SetCanBeCanceled(bCanBeCanceled);
	
}

void UFTAGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	TryActivateAbilityOnSpawn(ActorInfo, Spec);

}

void UFTAGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UFTAGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (bEnableTick)
	{
		TickTask = UFTAAT_OnTick::StartTicking(this);
		if (TickTask)
		{
			TickTask->OnTick.AddDynamic(this, &UFTAGameplayAbility::OnAbilityTick);
			TickTask->ReadyForActivation();
		}
	}
	QueueInputTask = UAT_WaitQueueInput::WaitQueueInput(this);
	if(QueueInputTask)
	{
		QueueInputTask->ReadyForActivation();
	}
}

void UFTAGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (TickTask)
	{
		TickTask->EndTask();
		TickTask = nullptr;
	}

}

bool UFTAGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}

	return true;
}

void UFTAGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	
}

FGameplayEffectContextHandle UFTAGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
	FGameplayEffectContextHandle ContextHandle = Super::MakeEffectContext(Handle, ActorInfo);
	
	// FFTAGameplayEffectContext* EffectContext = FFTAGameplayEffectContext::ExtractEffectContext(ContextHandle);
	//
	// if(!EffectContext)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Effect Context is Null"));
	// 	return ContextHandle;
	// }
	// check(EffectContext);
	// check(ActorInfo);
	//
	// AActor* EffectCauser = nullptr;
	// const IFTAAbilitySourceInterface* AbilitySource = nullptr;
	// float SourceLevel = 0.0f;
	// GetAbilitySource(Handle, ActorInfo, /*out*/ SourceLevel, /*out*/ AbilitySource, /*out*/ EffectCauser);
	//
	// UObject* SourceObject = GetSourceObject(Handle, ActorInfo);
	//
	// if(!SourceObject)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("SourceObject is Null"));
	// 	return ContextHandle;
	// }
	// AActor* Instigator = ActorInfo ? ActorInfo->OwnerActor.Get() : nullptr;
	//
	// if(!Instigator)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Instigator is Null"));
	// 	return ContextHandle;
	// }
	//
	// EffectContext->SetAbilitySource(AbilitySource, SourceLevel);
	// EffectContext->AddInstigator(Instigator, EffectCauser);
	// EffectContext->AddSourceObject(SourceObject);
	
	return ContextHandle;
}

void UFTAGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const
{
	Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);
}

bool UFTAGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// return Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags, OptionalRelevantTags);

	bool IsBlocked = false;
	bool IsMissing = false;

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	FGameplayTagContainer BlockContainer;
	AbilitySystemComponent.GetBlockedAbilityTags(BlockContainer);

	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
	{
		IsBlocked = true;
	}

	const UFTAAbilitySystemComponent* FTAASC = Cast<UFTAAbilitySystemComponent>(&AbilitySystemComponent);
	static FGameplayTagContainer AllRequiredTags;
	static FGameplayTagContainer AllBlockedTags;

	AllRequiredTags = ActivationRequiredTags;
	AllBlockedTags = ActivationBlockedTags;

	if (FTAASC)
	{
		FTAASC->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
	}
	
	if (AllBlockedTags.Num() || AllRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;
		
		AbilitySystemComponentTags.Reset();
		AbilitySystemComponent.GetOwnedGameplayTags(AllBlockedTags);

		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
		{
			IsBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
		{
			IsMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				IsBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				IsMissing = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				IsBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				IsMissing = true;
			}
		}
	}

	if (IsBlocked)
	{
		if (OptionalRelevantTags && BlockedTag.IsValid())
		{
			OptionalRelevantTags->AddTag(BlockedTag);
		}
		return false;
	}
	if (IsMissing)
	{
		if (OptionalRelevantTags && MissingTag.IsValid())
		{
			OptionalRelevantTags->AddTag(MissingTag);
		}
		return false;
	}

	return true;
	
}

void UFTAGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IFTAAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const
{
	OutSourceLevel = 0.0f;
	OutAbilitySource = nullptr;
	OutEffectCauser = nullptr;

	OutEffectCauser = ActorInfo->AvatarActor.Get();

	// If we were added by something that's an ability info source, use it
	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

	OutAbilitySource = Cast<IFTAAbilitySourceInterface>(SourceObject);
}
