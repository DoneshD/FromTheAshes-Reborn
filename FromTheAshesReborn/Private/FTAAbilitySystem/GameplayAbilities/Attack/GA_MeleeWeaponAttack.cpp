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
#include "InterchangeResult.h"
#include "Camera/CameraSystemComponent.h"
#include "Camera/CameraSystemParams.h"
#include "NiagaraSystem.h"
#include "FTAAbilitySystem/GameplayCues/SlashCueObject.h"
#include "FTAAbilitySystem/GameplayCues/HitCueObject.h"
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

	CentralStateComponent = FTAChar->CentralStateComponent;

	if(!CentralStateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - CentralStateComponent is Null"));
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

	
		MeleeWeaponActor->TracingComponent->BoxHalfSize = FVector(
			DefaultAttackData.WeaponTraceSizeStruct.WeaponTraceSize.X,
			DefaultAttackData.WeaponTraceSizeStruct.WeaponTraceSize.Y,
			DefaultAttackData.WeaponTraceSizeStruct.WeaponTraceSize.Z);
	
	
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

	AttackData.AttackDirectionStruct.AttackDirection = ESpatialDirection::None;


	if(!GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StateTreeTag.Status.Ability.MeleeAttack.Finished")))
	{
		GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("StateTreeTag.Status.Ability.MeleeAttack.Finished"));
		GetFTAAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("StateTreeTag.Status.Ability.MeleeAttack.Finished"));
	}

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

		AttackData = DefaultAttackData;
		
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
	MeleeWeaponActor->TracingComponent->BoxHalfSize = FVector(
		AttackData.WeaponTraceSizeStruct.WeaponTraceSize.X,
		AttackData.WeaponTraceSizeStruct.WeaponTraceSize.Y,
		AttackData.WeaponTraceSizeStruct.WeaponTraceSize.Z);
	
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

void UGA_MeleeWeaponAttack::SetRuntimeMeleeData(FMeleeAttackDataStruct InMeleeData)
{
	//Damage
	if(InMeleeData.ApplyDamageEffect && InMeleeData.ApplyDamageEffect->IsValidLowLevel())
	{
		AttackData.ApplyDamageEffect = InMeleeData.ApplyDamageEffect;
	}
	

	//Hit Reactions
	if (InMeleeData.PossibleHitReactions.Num() > 0 && !InMeleeData.PossibleHitReactions.IsEmpty())
	{
		for (TSubclassOf HitReaction : InMeleeData.PossibleHitReactions)
		{
			if(HitReaction && HitReaction->IsValidLowLevel())
			{
				if (!AttackData.PossibleHitReactions.Contains(HitReaction))
				{
					AttackData.PossibleHitReactions.Insert(HitReaction, 0);
				}
			}
		}
	}

	//Weapon trace

	if(InMeleeData.WeaponTraceSizeStruct.bEnabled)
	{
		AttackData.WeaponTraceSizeStruct.WeaponTraceSize = InMeleeData.WeaponTraceSizeStruct.WeaponTraceSize;
	}

	//Direction
	if(InMeleeData.AttackDirectionStruct.bEnabled)
	{
		if(InMeleeData.AttackDirectionStruct.AttackDirection != ESpatialDirection::None)
		{
			AttackData.AttackDirectionStruct.AttackDirection = InMeleeData.AttackDirectionStruct.AttackDirection;
		}
	}

	//Slash Cue
	if(InMeleeData.SlashCueClass && InMeleeData.SlashCueClass->IsValidLowLevel())
	{
		AttackData.SlashCueClass = InMeleeData.SlashCueClass;
	}

	//Hit Cue
	if(InMeleeData.HitCueClass && InMeleeData.HitCueClass->IsValidLowLevel())
	{
		AttackData.HitCueClass = InMeleeData.HitCueClass;
	}


}

void UGA_MeleeWeaponAttack::ExtractMeleeAssetProperties(TObjectPtr<UMeleeAbilityDataAsset> MeleeAsset)
{
	//Damage
	if(MeleeAsset->AttackData.ApplyDamageEffect && MeleeAsset->AttackData.ApplyDamageEffect->IsValidLowLevel())
	{
		AttackData.ApplyDamageEffect = MeleeAsset->AttackData.ApplyDamageEffect;
	}

	//Hit Reactions
	if(MeleeAsset->AttackData.PossibleHitReactions.Num() > 0 && !MeleeAsset->AttackData.PossibleHitReactions.IsEmpty())
	{
		for (TSubclassOf HitReaction : MeleeAsset->AttackData.PossibleHitReactions)
		{
			if(HitReaction && HitReaction->IsValidLowLevel())
			{
				if (!AttackData.PossibleHitReactions.Contains(HitReaction))
				{
					AttackData.PossibleHitReactions.Insert(HitReaction, 0);
				}
			}
			
		}
	}
	
	//Weapon trace
	if(MeleeAsset->AttackData.WeaponTraceSizeStruct.bEnabled)
	{
		AttackData.WeaponTraceSizeStruct.WeaponTraceSize = MeleeAsset->AttackData.WeaponTraceSizeStruct.WeaponTraceSize;
	}

	//Direction
	if(MeleeAsset->AttackData.AttackDirectionStruct.AttackDirection != ESpatialDirection::None)
	{
		AttackData.AttackDirectionStruct.AttackDirection = MeleeAsset->AttackData.AttackDirectionStruct.AttackDirection;
	}
	
	//Slash Cue
	if(MeleeAsset->AttackData.SlashCueClass && MeleeAsset->AttackData.SlashCueClass->IsValidLowLevel())
	{
		AttackData.SlashCueClass = MeleeAsset->AttackData.SlashCueClass;
	}

	//Hit Cue
	if(MeleeAsset->AttackData.HitCueClass && MeleeAsset->AttackData.HitCueClass->IsValidLowLevel())
	{
		AttackData.HitCueClass = MeleeAsset->AttackData.HitCueClass;
	}
	
}

void UGA_MeleeWeaponAttack::ExecuteMeleeHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	

	const FGameplayAbilityActorInfo* TargetActorInfo = TargetASC->AbilityActorInfo.Get();
	
	for(TSubclassOf HitAbilityClass : AttackData.PossibleHitReactions)
	{
		if(HitAbilityClass && HitAbilityClass->IsValidLowLevel())
		{
			
			
			const UGA_ReceiveHit* const CDO = HitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
			if (CDO)
			{
				
				GrantHitAbility(TargetDataHandle, HitAbilityClass);
				
				const FGameplayAbilitySpec* TargetSpec = TargetASC->FindAbilitySpecFromClass(HitAbilityClass);
				if(CDO->CanActivateAbility(TargetSpec->Handle, TargetActorInfo, nullptr, nullptr, nullptr))
				{
					ApplyMeleeHitEffects(TargetDataHandle, HitAbilityClass);
					SendMeleeHitGameplayEvents(TargetDataHandle, HitAbilityClass);
					AddMeleeHitCues(TargetDataHandle, HitAbilityClass);
					break;
				}
				else
				{
					
				}
			}
		}
	}
}

void UGA_MeleeWeaponAttack::ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	if (InHitAbilityClass->IsValidLowLevel())
	{
		const UGA_ReceiveHit* const CDO = InHitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(AttackData.ApplyDamageEffect)
			{
				TArray<FActiveGameplayEffectHandle> AppliedDamageEffects = ApplyGameplayEffectToTarget(
				CurrentSpecHandle,
				CurrentActorInfo,
				CurrentActivationInfo,
				TargetDataHandle,
				AttackData.ApplyDamageEffect, 
				1,
				1
				);
			}
			
			/*if(CDO->HitEffect)
			{
				FGameplayEffectSpecHandle HitEffectHandle = MakeOutgoingGameplayEffectSpec(CDO->HitEffect, 1.0f);

				TArray<FActiveGameplayEffectHandle> TestAppliedHitEffects = ApplyGameplayEffectSpecToTarget(
						CurrentSpecHandle,
						CurrentActorInfo,
						CurrentActivationInfo,
						HitEffectHandle,
						TargetDataHandle
					);
			}*/
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleeWeaponActor is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
}

void UGA_MeleeWeaponAttack::GrantHitAbility(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	if (InHitAbilityClass->IsValidLowLevel())
	{
		const UGA_ReceiveHit* const CDO = InHitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->ReceiveHitTag))
			{
				const TSubclassOf<UGameplayEffect>* GrantAbilityEffect = CDO->ReceiveHitEffectMap.Find(CDO->ReceiveHitTag);

				if(GrantAbilityEffect)
				{
					FGameplayEffectSpecHandle GrantAbilityEffectHandle = MakeOutgoingGameplayEffectSpec(*GrantAbilityEffect, 1.0f);

					TArray<FActiveGameplayEffectHandle> AppliedHitEffects = ApplyGameplayEffectSpecToTarget(
							CurrentSpecHandle,
							CurrentActorInfo,
							CurrentActivationInfo,
							GrantAbilityEffectHandle,
							TargetDataHandle
						);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::GrantHitAbility - GrantAbilityEffect is null"))
					EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
					return;
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::GrantHitAbility - Invalid tag"))
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::GrantHitAbility - Invalid CDO"))
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::GrantHitAbility - InHitAbilityClass is invalid"));
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
	
	HitInfoObj->HitData.HitDirection = AttackData.AttackDirectionStruct.AttackDirection;
	
	if(!MeleeWeaponActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleeWeaponActor is Null"));
		return;
	}
	
	if(!OnHitEventData.OptionalObject)
	{
		OnHitEventData.OptionalObject = HitInfoObj;
	}
	
	if (InHitAbilityClass)
	{
		const UGA_ReceiveHit* const CDO = InHitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->ReceiveHitTag))
			{
				OnHitEventData.EventTag = CDO->ReceiveHitTag;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - HitReactionTag is invalid"));
			}
		}
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OnHitEventData.EventTag, OnHitEventData);
	OnHitEventData.OptionalObject = nullptr;
}

void UGA_MeleeWeaponAttack::AddMeleeHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle, TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	

	FGameplayCueParameters HitCueParams;
	if(AttackData.SlashCueClass)
	{
		UHitCueObject* CueCDO = AttackData.HitCueClass->GetDefaultObject<UHitCueObject>();

		if(CueCDO)
		{
			HitCueParams.SourceObject = CueCDO;
			HitCueParams.EffectCauser = TargetActor;
			HitCueParams.Location = TargetDataHandle.Get(0)->GetHitResult()->Location;
			
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CueCDO->HitCueInfo.HitCueTag))
			{
				K2_AddGameplayCueWithParams(CueCDO->HitCueInfo.HitCueTag, HitCueParams);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::AddMeleeHitCues - HitCueTag is invalid"));
			}
		}
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
	
	FGameplayCueParameters SlashCueParams;
	if(AttackData.SlashCueClass)
	{
		USlashCueObject* CueCDO = AttackData.SlashCueClass->GetDefaultObject<USlashCueObject>();

		if(CueCDO)
		{
			SlashCueParams.SourceObject = CueCDO;

			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CueCDO->SlashCueInfo.SlashCueTag))
			{
				if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.BeginSlash")))
				{
					K2_AddGameplayCueWithParams(CueCDO->SlashCueInfo.SlashCueTag, SlashCueParams);
					StartMeleeWeaponTrace();
				}
				if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.EndSlash")))
				{
					K2_RemoveGameplayCue(CueCDO->SlashCueInfo.SlashCueTag);
					EndMeleeWeaponTrace();
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EventMontageReceived - SlashFXCueTag is invalid"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CueCDO null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::EventMontageReceived - Slash Cue ckass is invalid"));
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