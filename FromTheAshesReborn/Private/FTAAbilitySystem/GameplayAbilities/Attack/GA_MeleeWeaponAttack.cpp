#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack.h"
#include "AbilitySystemComponent.h"
#include "CombatComponents/ComboManagerComponent.h"
#include "MotionWarpingComponent.h"
#include "DataAsset/FTAAbilityDataAsset.h"
#include "DataAsset/MeleeAbilityDataAsset.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_PlayMontageAndWaitForEvent.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "Weapon/EquipmentManagerComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Camera/CameraSystemComponent.h"
#include "Camera/CameraSystemParams.h"
#include "NiagaraSystem.h"
#include "SWarningOrErrorBox.h"
#include "CombatComponents/CentralStateComponent.h"
#include "CombatComponents/MeleePropertiesComponent.h"
#include "EventObjects/HitEventObject.h"
#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"
#include "FTACustomBase/AfterImageActor.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TracingComponent/TracingComponent.h"


#include "Weapon/MeleeWeaponInstance.h"
#include "Weapon/WeaponActorBase.h"

UGA_MeleeWeaponAttack::UGA_MeleeWeaponAttack(const FObjectInitializer&)
{
	
}

void UGA_MeleeWeaponAttack::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
	
}


UMeleeWeaponInstance* UGA_MeleeWeaponAttack::GetMeleeWeaponInstance() const
{
	return CastChecked<UMeleeWeaponInstance>(GetAssociatedWeaponInstance());
}

bool UGA_MeleeWeaponAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		return false;
	}

	//Might change later

	if(Character->GetCharacterMovement()->IsFalling() || Character->GetCharacterMovement()->IsFlying())
	{
		return false;
	}

	return !Character->GetCharacterMovement()->IsFalling() || !Character->GetCharacterMovement()->IsFlying();
}

void UGA_MeleeWeaponAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FTAChar = Cast<AFTACharacter>(GetAvatarActorFromActorInfo());

	if(!FTAChar)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - Avatar Actor Not FTAChar"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	ComboManagerComponent = FTAChar->ComboManagerComponent;

	if(!ComboManagerComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - ComboManagerComponent is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	if(!FTAChar->EquipmentManagerComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - EquipmentManagerComponent is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	MeleeWeaponActor = FTAChar->EquipmentManagerComponent->GetEquippedWeaponActor();
	
	if(!MeleeWeaponActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleeWeaponActor is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	MeleePropertiesComponent = FTAChar->MeleePropertiesComponent;

	if(!MeleePropertiesComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleePropertiesComponent is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	MeleePropertiesComponent->OnSetMeleeData.AddUniqueDynamic(this, &UGA_MeleeWeaponAttack::SetRuntimeMeleeData);
	
	MeleeWeaponActor->TracingComponent->OnItemAdded.AddDynamic(this, &UGA_MeleeWeaponAttack::OnHitAdded);
	MeleeWeaponActor->TracingComponent->BoxHalfSize = FVector(FinalAttackData.WeaponTraceSize.X, FinalAttackData.WeaponTraceSize.Y, FinalAttackData.WeaponTraceSize.Z);
	
	if(!MeleeAttackAssets.NormalAttacks.IsValidIndex(0) || MeleeAttackAssets.NormalAttacks.Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Melee Attack Assets is invalid"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	PerformMeleeAttack(MeleeAttackAssets);

	UCameraSystemComponent* CSC = GetFTACharacterFromActorInfo()->FindComponentByClass<UCameraSystemComponent>();
	if(!CSC)
	{
		return;
	}
	
}

void UGA_MeleeWeaponAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeWeaponAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if(!MeleeWeaponActor)
	{
		//TODO: Printing incorrectly, fix later
		//UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EndAbility - GetFTACharacterFromActorInfo()->CurrentWeapon is invalid"))
		return;
	}
	
	MeleeWeaponActor->TracingComponent->OnItemAdded.RemoveAll(this);

	UCameraSystemComponent* CSC = GetFTACharacterFromActorInfo()->FindComponentByClass<UCameraSystemComponent>();
	if(!CSC)
	{
		return;
	}

}

void UGA_MeleeWeaponAttack::ResetMeleeAttack()
{
	ComboManagerComponent->GetCurrentComboContainer().Reset();
	ComboManagerComponent->SetCurrentComboIndex(0);
	ComboManagerComponent->PauseCurrentAttack = false;
}

void UGA_MeleeWeaponAttack::PerformMeleeAttack(FMeleeAttackForms& MeleeAttackDataAssets)
{
	if(NonMontageAbility)
	{
		return;
	}
	
	TObjectPtr<UMeleeAbilityDataAsset> MatchingDataAsset;
	bool DataAssetFound = ComboManagerComponent->FindMatchingMeleeAssetToTagContainer(MeleeAttackDataAssets, MatchingDataAsset);
	
	if(!DataAssetFound)
	{
		UE_LOG(LogTemp, Error, TEXT("DA NOT found"));
		ResetMeleeAttack();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	if(!MatchingDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("MatchingDataAsset is NULL"));
		ResetMeleeAttack();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	if (MatchingDataAsset->MontageToPlay)
	{
		int32 CurrentComboIndex = ComboManagerComponent->GetCurrentComboIndex();
		
		ComboManagerComponent->GetCurrentComboContainer().AddTag(MatchingDataAsset->UniqueIdentifierTag);
		ComboManagerComponent->SetCurrentComboIndex(CurrentComboIndex + 1);
		ComboManagerComponent->PauseCurrentAttack = false;

		FinalAttackData = DefaultAttackData;
		
		ExtractMeleeAssetProperties(MatchingDataAsset);
		PlayAbilityAnimMontage(MatchingDataAsset->MontageToPlay);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AttackMontageToPlay is NULL"));
		ResetMeleeAttack();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}                                                                                 
}

void UGA_MeleeWeaponAttack::StartMeleeWeaponTrace()
{
	MeleeWeaponActor->TracingComponent->BoxHalfSize = FVector(FinalAttackData.WeaponTraceSize.X, FinalAttackData.WeaponTraceSize.Y, FinalAttackData.WeaponTraceSize.Z);
	MeleeWeaponActor->TracingComponent->ToggleTraceCheck(true);
}

void UGA_MeleeWeaponAttack::EndMeleeWeaponTrace()
{
	MeleeWeaponActor->TracingComponent->ToggleTraceCheck(false);
	MeleeWeaponActor->TracingComponent->ClearHitArray();
}

void UGA_MeleeWeaponAttack::OnHitAdded(FHitResult LastItem)
{
	AActor* TargetActor = LastItem.GetActor();

	if (TargetActor && TargetActor->Implements<UAbilitySystemInterface>())
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(TargetActor);
		UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent();

		//TODO: Temporary, change later
		AEnemyBaseCharacter* Enemy = Cast<AEnemyBaseCharacter>(Cast<AEnemyBaseCharacter>(GetFTACharacterFromActorInfo()));

		if(Enemy)
		{
			AEnemyBaseCharacter* TargetEnemy = Cast<AEnemyBaseCharacter>(TargetActor);
			if(TargetEnemy)
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
				return;
			}
		}

		if (TargetASC)
		{
			FGameplayAbilityTargetDataHandle TargetHitDataHandle = AddHitResultToTargetData(LastItem);
			if(TargetHitDataHandle.Num() > 0 && TargetHitDataHandle.Get(0))
			{
				ExecuteMeleeHitLogic(TargetHitDataHandle);
			}
		}
	}
}

FGameplayAbilityTargetDataHandle UGA_MeleeWeaponAttack::AddHitResultToTargetData(const FHitResult& LastItem)
{
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(LastItem);

	if(!TargetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Target Data"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return nullptr;
	}
	
	TargetData->HitResult = LastItem;
	
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(TargetData);
	return TargetDataHandle;
	
}

void UGA_MeleeWeaponAttack::RemoveHitReaction(FGameplayTag RemovalTag)
{
	for (int32 i = 0; i < FinalAttackData.PossibleHitReactions.Num(); i++)
	{
		const TSubclassOf<UGA_ReceiveHit> ReceiveHitClass = FinalAttackData.PossibleHitReactions[i];
		if (ReceiveHitClass)
		{
			const UGA_ReceiveHit* const CDO = ReceiveHitClass->GetDefaultObject<UGA_ReceiveHit>();
			if (CDO && UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->CharacterOrientationTag))
			{
				if (CDO->CharacterOrientationTag.MatchesTagExact(RemovalTag))
				{
					FinalAttackData.PossibleHitReactions.RemoveAt(i);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::RemoveHitReaction - Invalid CDO or Tag on class %s"), *GetNameSafe(*ReceiveHitClass));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::RemoveHitReaction - Null ReceiveHit class at index %d"), i);
		}
	}
}

void UGA_MeleeWeaponAttack::SelectHitReaction(UAbilitySystemComponent* TargetASC, TSubclassOf<UGA_ReceiveHit>& InHitAbilityClass)
{
	if (TargetASC->HasMatchingGameplayTag(GetFTACharacterFromActorInfo()->CentralStateComponent->GroundedTag))
	{
		RemoveHitReaction(GetFTACharacterFromActorInfo()->CentralStateComponent->AirborneTag);
	}
	
	if (TargetASC->HasMatchingGameplayTag(GetFTACharacterFromActorInfo()->CentralStateComponent->AirborneTag))
	{
		RemoveHitReaction(GetFTACharacterFromActorInfo()->CentralStateComponent->GroundedTag);
	}
	
	if (TargetASC->HasMatchingGameplayTag(GetFTACharacterFromActorInfo()->CentralStateComponent->NeutralTag))
	{
		RemoveHitReaction(GetFTACharacterFromActorInfo()->CentralStateComponent->DownedTag);
	}

	if (TargetASC->HasMatchingGameplayTag(GetFTACharacterFromActorInfo()->CentralStateComponent->DownedTag))
	{
		RemoveHitReaction(GetFTACharacterFromActorInfo()->CentralStateComponent->NeutralTag);
	}
	
	if(FinalAttackData.PossibleHitReactions.Num() > 1)
	{
		InHitAbilityClass = FinalAttackData.PossibleHitReactions.Last();
		return;
	}

	if(FinalAttackData.PossibleHitReactions.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SelectHitReaction - No possible hit reactions"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	InHitAbilityClass = FinalAttackData.PossibleHitReactions[0];
}

bool UGA_MeleeWeaponAttack::GetTargetStateComponentsAndHitReaction(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit>& InHitAbilityClass)
{
	const FGameplayAbilityTargetData* HitTargetData = TargetDataHandle.Get(0);
	
	if(!HitTargetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeWeaponAttack::SelectHitReaction - No Target Data"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return false;
	}
	
	if(HitTargetData->GetActors().Num() < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Target Actors"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return false;
	}
	
	TArray<TWeakObjectPtr<AActor>> AllHitActors = HitTargetData->GetActors();
	AActor* HitActor = AllHitActors[0].Get();
	
	if(!HitActor->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Hit Actor"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return false;
	}
	
	if (HitActor->Implements<UAbilitySystemInterface>())
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(HitActor);

		if(UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent())
		{
			SelectHitReaction(TargetASC, InHitAbilityClass);
			return true;
		}
	}
	return false;
}

void UGA_MeleeWeaponAttack::SetRuntimeMeleeData(FMeleeAttackDataStruct InMeleeData)
{
	if(InMeleeData.HitFX)
	{
		FinalAttackData.HitFX = InMeleeData.HitFX;
	}
	
	if(InMeleeData.SlashFX)
	{
		FinalAttackData.HitFX = InMeleeData.SlashFX;
	}

	if(InMeleeData.AttackDirection != ESpatialDirection::None)
	{
		FinalAttackData.AttackDirection = InMeleeData.AttackDirection;
	}

	FinalAttackData.WeaponTraceSize = InMeleeData.WeaponTraceSize;

	if(InMeleeData.PossibleHitReactions.Num() > 0)
	{
		for (TSubclassOf HitReaction : InMeleeData.PossibleHitReactions)
		{
			FinalAttackData.PossibleHitReactions.AddUnique(HitReaction);
		}
	}
}

void UGA_MeleeWeaponAttack::ExtractMeleeAssetProperties(TObjectPtr<UMeleeAbilityDataAsset> MeleeAsset)
{
	if(!FinalAttackData.HitFX->IsValidLowLevel())
	{
		FinalAttackData.HitFX = MeleeAsset->AttackData.HitFX;
	}

	if(!FinalAttackData.SlashFX->IsValidLowLevel())
	{
		FinalAttackData.SlashFX = MeleeAsset->AttackData.SlashFX;
	}
	
	FinalAttackData.WeaponTraceSize = MeleeAsset->AttackData.WeaponTraceSize;

	if(FinalAttackData.AttackDirection != ESpatialDirection::None)
	{
		FinalAttackData.AttackDirection = MeleeAsset->AttackData.AttackDirection;
	}

	if(MeleeAsset->AttackData.PossibleHitReactions.Num() > 0)
	{
		for (TSubclassOf HitReaction : MeleeAsset->AttackData.PossibleHitReactions)
		{
			FinalAttackData.PossibleHitReactions.AddUnique(HitReaction);
		}
	}
}

void UGA_MeleeWeaponAttack::ExecuteMeleeHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	TSubclassOf<UGA_ReceiveHit> OutHitAbilityClass;
	bool FoundHitReaction = GetTargetStateComponentsAndHitReaction(TargetDataHandle, OutHitAbilityClass);

	if(!FoundHitReaction)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeWeaponAttack::ExecuteMeleeHitLogic - Hit reaction not found"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	ApplyMeleeHitEffects(TargetDataHandle, OutHitAbilityClass);
	SendMeleeHitGameplayEvents(TargetDataHandle, OutHitAbilityClass);
	AddMeleeHitCues(TargetDataHandle, OutHitAbilityClass);
	
}

void UGA_MeleeWeaponAttack::ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	if (InHitAbilityClass->IsValidLowLevel())
	{
		const UGA_ReceiveHit* const CDO = InHitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(FinalAttackData.ApplyDamageEffect)
			{
				TArray<FActiveGameplayEffectHandle> AppliedDamageEffects = ApplyGameplayEffectToTarget(
				CurrentSpecHandle,
				CurrentActorInfo,
				CurrentActivationInfo,
				TargetDataHandle,
				FinalAttackData.ApplyDamageEffect, 
				1,
				1
				);
			}
			
			if(CDO->HitEffect)
			{
				FGameplayEffectSpecHandle HitEffectHandle = MakeOutgoingGameplayEffectSpec(CDO->HitEffect, 1.0f);

				TArray<FActiveGameplayEffectHandle> TestAppliedHitEffects = ApplyGameplayEffectSpecToTarget(
						CurrentSpecHandle,
						CurrentActorInfo,
						CurrentActivationInfo,
						HitEffectHandle,
						TargetDataHandle
					);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleeWeaponActor is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
}

void UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	
	OnHitEventData.Instigator = GetAvatarActorFromActorInfo();
	OnHitEventData.Target = TargetActor;
	OnHitEventData.ContextHandle.AddHitResult(*TargetDataHandle.Get(0)->GetHitResult());
	
	UHitEventObject* HitInfoObj = NewObject<UHitEventObject>(this);
	HitInfoObj->HitData.Instigator = GetAvatarActorFromActorInfo();
	
	if(!MeleeWeaponActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleeWeaponActor is Null"));
		return;
	}
	
	OnHitEventData.OptionalObject = HitInfoObj;

	if (InHitAbilityClass)
	{
		const UGA_ReceiveHit* const CDO = InHitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			HitInfoObj->HitData.HitDirection = FinalAttackData.AttackDirection;
			
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->HitTag))
			{
				OnHitEventData.EventTag = CDO->HitTag;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - HitReactionTag is invalid"));
			}
		}
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OnHitEventData.EventTag, OnHitEventData);
}

void UGA_MeleeWeaponAttack::AddMeleeHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	
	FGameplayCueParameters HitCueParams;
	if(FinalAttackData.HitFX)
	{
		HitCueParams.SourceObject = static_cast<const UObject*>(FinalAttackData.HitFX);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::AddMeleeHitCues - HitFX is invalid"));

	}
	HitCueParams.EffectCauser = TargetActor;
	HitCueParams.Location = TargetDataHandle.Get(0)->GetHitResult()->Location;

	if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(FinalAttackData.HitFXCueTag))
	{
		K2_AddGameplayCueWithParams(FinalAttackData.HitFXCueTag, HitCueParams);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::AddMeleeHitCues - HitFXCueTag is invalid"));

	}
}

void UGA_MeleeWeaponAttack::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
	
}

void UGA_MeleeWeaponAttack::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
	GetFTACharacterFromActorInfo()->MotionWarpingComponent->RemoveWarpTarget(WarpTargetName);
}

void UGA_MeleeWeaponAttack::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

	ResetMeleeAttack();
	GetFTACharacterFromActorInfo()->MotionWarpingComponent->RemoveWarpTarget(WarpTargetName);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_MeleeWeaponAttack::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
	
	if(!UTagValidationFunctionLibrary::IsRegisteredGameplayTag(EventTag))
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EventMontageReceived - EventTag is invalid"))
		return;
	}
	
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("QueueTag.PauseCurrentAttack")))
	{
		ComboManagerComponent->PauseCurrentAttack = true;
	}

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.BeginSlash")))
	{
		FGameplayCueParameters SlashCueParams;
		if(FinalAttackData.SlashFX)
		{
			SlashCueParams.SourceObject = static_cast<const UObject*>(FinalAttackData.SlashFX);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EventMontageReceived - SlashFX is invalid"));
		}
		if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(FinalAttackData.SlashFXCueTag))
		{
			K2_AddGameplayCueWithParams(FinalAttackData.SlashFXCueTag, SlashCueParams);
			StartMeleeWeaponTrace();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EventMontageReceived - SlashFXCueTag is invalid"));
		}
	}

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.EndSlash")))
	{
		if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(FinalAttackData.SlashFXCueTag))
		{
			K2_RemoveGameplayCue(FinalAttackData.SlashFXCueTag);
			EndMeleeWeaponTrace();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EventMontageReceived - SlashFXCueTag is invalid"));
		}
	}
}

void UGA_MeleeWeaponAttack::SpawnAfterImage()
{
	FVector SpawnLocation = FTAChar->GetMesh()->GetComponentLocation();
	FRotator SpawnRotation = FTAChar->GetMesh()->GetComponentRotation();

	AActor* Image = GetWorld()->SpawnActor<AActor>(FTAChar->AfterImageActor, SpawnLocation, SpawnRotation);

	if(!Image)
	{
		UE_LOG(LogTemp, Warning, TEXT("Image is Null"));
		return;
	}

	AAfterImageActor* AfterImage = Cast<AAfterImageActor>(Image);
	if(!AfterImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("AfterImageActor is Null"));
		return;
	}

	AfterImage->MeshToCopy = FTAChar->GetMesh();

	if(!AfterImage->MeshToCopy)
	{
		UE_LOG(LogTemp, Warning, TEXT("AfterImage->MeshToCopy is Null"));
		return;
	}

	AfterImage->SpawnAfterImageActor();
}