#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTypes/FTATargetType.h"
#include "GameplayTagContainer.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "Components/Combat/PlayerComboManagerComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "Player/FTAPlayerController.h"


UFTAGameplayAbility::UFTAGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bActivateAbilityOnGranted = false;
	bActivateOnInput = true;

	// ActivationPolicy = ELyraAbilityActivationPolicy::OnInputTriggered;
	// ActivationGroup = ELyraAbilityActivationGroup::Independent;
	
}

UFTAAbilitySystemComponent* UFTAGameplayAbility::GetFTAAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UFTAAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AFTAPlayerController* UFTAGameplayAbility::GetFTAPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AFTAPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
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

void UFTAGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

FGameplayAbilityTargetDataHandle UFTAGameplayAbility::MakeGameplayAbilityTargetDataHandleFromActorArray(const TArray<AActor*> TargetActors)
{
	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		return FGameplayAbilityTargetDataHandle(NewData);
	}

	return FGameplayAbilityTargetDataHandle();
}

FGameplayAbilityTargetDataHandle UFTAGameplayAbility::MakeGameplayAbilityTargetDataHandleFromHitResults(const TArray<FHitResult> HitResults)
{
	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	return TargetData;
}

FFTAGameplayEffectContainerSpec UFTAGameplayAbility::MakeEffectContainerSpecFromContainer(const FFTAGameplayEffectContainer& Container, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	// First figure out our actor info
	FFTAGameplayEffectContainerSpec ReturnSpec;
	AActor* OwningActor = GetOwningActorFromActorInfo();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	AFTACharacter* AvatarCharacter = Cast<AFTACharacter>(AvatarActor);
	UFTAAbilitySystemComponent* OwningASC = UFTAAbilitySystemComponent::GetAbilitySystemComponentFromActor(OwningActor);

	if (OwningASC)
	{
		// If we have a target type, run the targeting logic. This is optional, targets can be added later
		if (Container.TargetType.Get())
		{
			TArray<FHitResult> HitResults;
			TArray<AActor*> TargetActors;
			TArray<FGameplayAbilityTargetDataHandle> TargetData;
			const UFTATargetType* TargetTypeCDO = Container.TargetType.GetDefaultObject();
			TargetTypeCDO->GetTargets(AvatarCharacter, AvatarActor, EventData, TargetData, HitResults, TargetActors);
			ReturnSpec.AddTargets(TargetData, HitResults, TargetActors);
		}

		// If we don't have an override level, use the ability level
		if (OverrideGameplayLevel == INDEX_NONE)
		{
			//OverrideGameplayLevel = OwningASC->GetDefaultAbilityLevel();
			OverrideGameplayLevel = GetAbilityLevel();
		}

		// Build GameplayEffectSpecs for each applied effect
		for (const TSubclassOf<UGameplayEffect>& EffectClass : Container.TargetGameplayEffectClasses)
		{
			ReturnSpec.TargetGameplayEffectSpecs.Add(MakeOutgoingGameplayEffectSpec(EffectClass, OverrideGameplayLevel));
		}
	}
	return ReturnSpec;
}



FFTAGameplayEffectContainerSpec UFTAGameplayAbility::MakeEffectContainerSpec(FGameplayTag ContainerTag, const FGameplayEventData& EventData, int32 OverrideGameplayLevel)
{
	
	// FFTAGameplayEffectContainerSpec* FoundContainer = EffectContainerMap.Find(ContainerTag);
	//
	// if (FoundContainer)
	// {
	// 	return MakeEffectContainerSpecFromContainer(*FoundContainer, EventData, OverrideGameplayLevel);
	// }
	//
	return FFTAGameplayEffectContainerSpec();
	
}

TArray<FActiveGameplayEffectHandle> UFTAGameplayAbility::ApplyEffectContainerSpec(const FFTAGameplayEffectContainerSpec& ContainerSpec)
{
	TArray<FActiveGameplayEffectHandle> AllEffects;

	// Iterate list of effect specs and apply them to their target data
	for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
	{
		AllEffects.Append(K2_ApplyGameplayEffectSpecToTarget(SpecHandle, ContainerSpec.TargetData));
	}
	return AllEffects;
}

UObject* UFTAGameplayAbility::K2_GetSourceObject(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const
{
	return GetSourceObject(Handle, &ActorInfo);
}

bool UFTAGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

bool UFTAGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) && FTACheckCost(Handle, *ActorInfo);
}


bool UFTAGameplayAbility::FTACheckCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const
{
	return true;
}

void UFTAGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	FTAApplyCost(Handle, *ActorInfo, ActivationInfo);
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}


bool UFTAGameplayAbility::IsInputPressed() const
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	return Spec && Spec->InputPressed;
}
