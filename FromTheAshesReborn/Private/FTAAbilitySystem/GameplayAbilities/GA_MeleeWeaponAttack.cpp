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

	UMeleeWeaponInstance* WeaponData = GetMeleeWeaponInstance();
	
	for (AActor* SpawnedActor : WeaponData->GetSpawnedActors())
	{
		//Using interface to call toggle function
		AWeaponActorBase* WeaponActor = Cast<AWeaponActorBase>(SpawnedActor);
		if(WeaponActor)
		{
			WeaponActor->DidItHitActorComponent->OnItemAdded.AddDynamic(this, &UGA_MeleeWeaponAttack::OnHitAdded);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT AWeaponActorBase"));
		}
	}
	
}

void UGA_MeleeWeaponAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UMeleeWeaponInstance* WeaponData = GetMeleeWeaponInstance();
	
	for (AActor* SpawnedActor : WeaponData->GetSpawnedActors())
	{
		//Using interface to call toggle function
		AWeaponActorBase* WeaponActor = Cast<AWeaponActorBase>(SpawnedActor);
		if(WeaponActor)
		{
			WeaponActor->DidItHitActorComponent->OnItemAdded.RemoveAll(this);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT AWeaponActorBase"));
		}
	}
}

void UGA_MeleeWeaponAttack::PerformLocalTargeting(TArray<FHitResult>& OutHits)
{
	UMeleeWeaponInstance* WeaponData = GetMeleeWeaponInstance();
	
	for (AActor* SpawnedActor : WeaponData->GetSpawnedActors())
	{
		//Using interface to call toggle function
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
	
	UMeleeWeaponInstance* WeaponData = GetMeleeWeaponInstance();
	
	for (AActor* SpawnedActor : WeaponData->GetSpawnedActors())
	{
		//Using interface to call toggle function
		AWeaponActorBase* WeaponActor = Cast<AWeaponActorBase>(SpawnedActor);
		if(WeaponActor)
		{
			// WeaponActor->DidItHitActorComponent->OnItemAdded.AddDynamic(this, &UGA_MeleeWeaponAttack::OnHitAdded);
			WeaponActor->DidItHitActorComponent->ToggleTraceCheck(true);
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT AWeaponActorBase"));
		}
	}
	
}

void UGA_MeleeWeaponAttack::EndMeleeWeaponTargeting()
{
	UMeleeWeaponInstance* WeaponData = GetMeleeWeaponInstance();
	for (AActor* SpawnedActor : WeaponData->GetSpawnedActors())
	{
		AWeaponActorBase* WeaponActor = Cast<AWeaponActorBase>(SpawnedActor);
		if(WeaponActor)
		{
			TArray<FHitResult> DIHArray = WeaponActor->DidItHitActorComponent->HitArray;

			for (FHitResult Element : DIHArray)
			{
				UE_LOG(LogTemp, Warning, TEXT("HitResult Name: %s"), *Element.HitObjectHandle.GetName())
			}
			
			WeaponActor->DidItHitActorComponent->ToggleTraceCheck(false);
			WeaponActor->DidItHitActorComponent->ClearHitArray();
			return;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT AWeaponActorBase"));

		}
	}
}

void UGA_MeleeWeaponAttack::OnHitAdded(FHitResult LastItem)
{
	UE_LOG(LogTemp, Warning, TEXT("HIT"));

	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(LastItem);
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(TargetData);

	OnMeleeWeaponTargetDataReady(TargetDataHandle);
}
