#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "FTAAbilitySystem/AbilityTypes/FTATargetType.h"
#include "GameplayTagContainer.h"
#include "MotionWarpingComponent.h"
#include "FTAAbilitySystem/FTAAbilitySourceInterface.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/AT_WaitInputTagAndQueueWindowEvent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_OnTick.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"
#include "FTACustomBase/FTACharacter.h"
#include "Player/FTAPlayerController.h"


UFTAGameplayAbility::UFTAGameplayAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bActivateAbilityOnGranted = false;
	bActivateOnInput = true;
	ActivationPolicy = EFTAAbilityActivationPolicy::OnInputTriggered;

	bIsCancelable = true;
	
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
	AActor* OwningActor = CurrentActorInfo->OwnerActor.Get();
	while (OwningActor)
	{
		if (AController* Controller = Cast<AController>(OwningActor))
		{
			return Controller;
		}

		if (APawn* Pawn = Cast<APawn>(OwningActor))
		{
			return Pawn->GetController();
		}

		OwningActor = OwningActor->GetOwner();
	}
	return nullptr;
}

AFTACharacter* UFTAGameplayAbility::GetFTACharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AFTACharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

void UFTAGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	if (ActorInfo && !Spec.IsActive() && ActivationPolicy == EFTAAbilityActivationPolicy::OnSpawn)
	{
		UFTAAbilitySystemComponent* FTAASC = GetFTAAbilitySystemComponentFromActorInfo();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		if(!FTAASC)
		{
			UE_LOG(LogTemp, Error, TEXT("UFTAGameplayAbility::TryActivateAbilityOnSpawn - FTAASC is null"));
			return;
		}
		
		FTAASC->TryActivateAbility(Spec.Handle);
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

bool UFTAGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
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

	// if (FTAASC->IsActivationGroupTagBlocked(ActivationGroupTag))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Blocked"))
	// 	return false;
	// }
	

	bool bHasActiveAbilities = false;

	for (const FGameplayAbilitySpec& Spec : FTAASC->GetActivatableAbilities())
	{
		if (Spec.IsActive())
		{
			UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(Spec.Ability);
			if(FTAAbility && FTAAbility->DefaultActivationGroupTag == ActivationBlockingTag)
			{
				bHasActiveAbilities = true;
				break;
			}
		}
	}

	if (bHasActiveAbilities && QueueWindowTag.IsValid())
	{
		if (!FTAASC->HasMatchingGameplayTag(QueueWindowTag))
		{
			return false;
		}
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

	//TODO: check if owner is player first
	WaitInputTagAndQueueWindowEventTask = UAT_WaitInputTagAndQueueWindowEvent::WaitInputTagAndQueueWindowEvent(this);
	if (WaitInputTagAndQueueWindowEventTask)
	{
		
		WaitInputTagAndQueueWindowEventTask->ReadyForActivation();
	}
}

void UFTAGameplayAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
}

void UFTAGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (TickTask)
	{
		TickTask->EndTask();
		TickTask = nullptr;
	}

	if(WaitInputTagAndQueueWindowEventTask)
	{
		WaitInputTagAndQueueWindowEventTask->EndTask();
		WaitInputTagAndQueueWindowEventTask = nullptr;
	}

	ActivationGroupTag = DefaultActivationGroupTag;

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
	// if(!ActorInfo)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("ActorInfo is Null"));
	// 	return ContextHandle;
	// }
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
	//
	return ContextHandle;
}

void UFTAGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const
{
	Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);
}

bool UFTAGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	

	bool IsBlocked = false;
	bool IsMissing = false;

	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	FGameplayTagContainer BlockContainer;
	AbilitySystemComponent.GetBlockedAbilityTags(BlockContainer);

	if (AbilitySystemComponent.AreAbilityTagsBlocked(GetAssetTags()))
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
		FTAASC->GetAdditionalActivationTagRequirements(GetAssetTags(), AllRequiredTags, AllBlockedTags);
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

	return Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags, OptionalRelevantTags);;
	
}

void UFTAGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IFTAAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const
{
	OutSourceLevel = 0.0f;
	OutAbilitySource = nullptr;
	OutEffectCauser = nullptr;

	OutEffectCauser = ActorInfo->AvatarActor.Get();

	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

	OutAbilitySource = Cast<IFTAAbilitySourceInterface>(SourceObject);
}

void UFTAGameplayAbility::MotionWarpToTarget()
{
	
}

void UFTAGameplayAbility::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	if(!AnimMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("UFTAGameplayAbility::PlayAbilityAnimMontage - Anim montage is invalid"));
		return;
	}
	PlayMontageTask = UFTAAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, AnimMontage, FGameplayTagContainer(),
	1.0f, NAME_None, false, 1.0f);

	
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UFTAGameplayAbility::OnMontageBlendingOut);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UFTAGameplayAbility::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UFTAGameplayAbility::OnMontageCancelled);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UFTAGameplayAbility::OnMontageCancelled);
	PlayMontageTask->EventReceived.AddDynamic(this, &UFTAGameplayAbility::EventMontageReceived);
	
	MotionWarpToTarget();

	PlayMontageTask->ReadyForActivation();

}

void UFTAGameplayAbility::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UFTAGameplayAbility::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	GetFTACharacterFromActorInfo()->MotionWarpingComponent->RemoveWarpTarget(WarpTargetName);
}

void UFTAGameplayAbility::OnMontageBlendingOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UFTAGameplayAbility::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}
