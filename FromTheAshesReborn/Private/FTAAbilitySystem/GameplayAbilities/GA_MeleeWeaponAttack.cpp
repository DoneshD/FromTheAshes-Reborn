#include "FTAAbilitySystem/GameplayAbilities/GA_MeleeWeaponAttack.h"

#include "AbilitySystemComponent.h"
#include "DidItHitActorComponent.h"
#include "Weapon/MeleeWeaponInstance.h"
#include "Weapon/WeaponActorBase.h"

UGA_MeleeWeaponAttack::UGA_MeleeWeaponAttack(const FObjectInitializer&)
{
}

UMeleeWeaponInstance* UGA_MeleeWeaponAttack::GetMeleeWeaponInstance() const
{
	return Cast<UMeleeWeaponInstance>(GetAssociatedWeaponInstance());
}

bool UGA_MeleeWeaponAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_MeleeWeaponAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	// OnTargetDataReadyCallbackDelegateHandle = ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_MeleeWeaponAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeWeaponAttack::PerformLocalTargeting(TArray<FHitResult>& Array)
{
	// APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	
	UMeleeWeaponInstance* WeaponData = GetMeleeWeaponInstance();
	// if (AvatarPawn && WeaponData)
	// {
	// 	FMeleeWeaponTraceData TraceData;
	// 	TraceData.WeaponData = WeaponData;
	// 	WeaponData->GetSpawnedActors()
	// 	// TraceData.StartTrace = 
	//
	// 	// InputData.EndAim = 
	// 	
	// 	// TraceBulletsInCartridge(TraceData, /*out*/ OutHits);
	// }


	for (AActor* SpawnedActor : WeaponData->GetSpawnedActors())
	{
		AWeaponActorBase* WeaponActor = Cast<AWeaponActorBase>(SpawnedActor);
		if(WeaponActor)
		{
			WeaponActor->DidItHitActorComponent->ToggleTraceCheck(true);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT AWeaponActorBase"));

		}
	}
}


void UGA_MeleeWeaponAttack::StartMeleeWeaponTargeting()
{
	check(CurrentActorInfo);
	
	TArray<FHitResult> FoundHits;
	PerformLocalTargeting(/*out*/ FoundHits);

	// Fill out the target data from the hit results
	FGameplayAbilityTargetDataHandle TargetData;

	// if (FoundHits.Num() > 0)
	// {
	// 	for (const FHitResult& FoundHit : FoundHits)
	// 	{
	// 		FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	// 		NewTargetData->HitResult = FoundHit;
	//
	// 		TargetData.Add(NewTargetData);
	// 	}
	// }
	// OnTargetDataReadyCallback(TargetData, FGameplayTag());
}

void UGA_MeleeWeaponAttack::EndMeleeWeaponTargeting()
{
	UMeleeWeaponInstance* WeaponData = GetMeleeWeaponInstance();
	for (AActor* SpawnedActor : WeaponData->GetSpawnedActors())
	{
		AWeaponActorBase* WeaponActor = Cast<AWeaponActorBase>(SpawnedActor);
		if(WeaponActor)
		{
			WeaponActor->DidItHitActorComponent->ToggleTraceCheck(false);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT AWeaponActorBase"));

		}
	}
}

void UGA_MeleeWeaponAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	
}
