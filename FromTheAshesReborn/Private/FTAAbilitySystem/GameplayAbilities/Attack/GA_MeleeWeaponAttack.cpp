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
#include "CentralStateComponent.h"
#include "Camera/CameraSystemComponent.h"
#include "Camera/CameraSystemParams.h"
#include "NiagaraSystem.h"
#include "SWarningOrErrorBox.h"
#include "CombatComponents/MeleePropertiesComponent.h"
#include "EventObjects/HitEventObject.h"
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

void UGA_MeleeWeaponAttack::SetRuntimeMeleeData(FMeleeRuntimeDataStruct InMeleeData)
{
	if(InMeleeData.HitFX)
	{
		CurrentHitFX = InMeleeData.HitFX;
	}
	
	if(InMeleeData.SlashFX)
	{
		CurrentSlashFX = InMeleeData.SlashFX;
	}

	if(InMeleeData.TraceSize)
	{
		CurrentTraceSize = InMeleeData.TraceSize;
	}

	if(InMeleeData.HitDirection != ESpatialDirection::None)
	{
		CurrentHitDirection = InMeleeData.HitDirection;
	}

	if(InMeleeData.HitReactionEffect)
	{
		CurrentHitReactionEffect = InMeleeData.HitReactionEffect;
	}
	
	if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(InMeleeData.HitReactionTag))
	{
		CurrentHitReactionTag = InMeleeData.HitReactionTag;
	}
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
	MeleeWeaponActor->TracingComponent->BoxHalfSize = FVector(TraceSize, TraceSize, TraceSize);
	
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

		ExtractMeleeAssetProperties(MatchingDataAsset);
		PlayAbilityAnimMontage(MatchingDataAsset->MontageToPlay);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AttackMontageToPlay is NULL"));
		ResetMeleeAttack();
	}                                                                                 
}

void UGA_MeleeWeaponAttack::StartMeleeWeaponTrace()
{
	if(CurrentTraceSize > 0.0f)
	{
		MeleeWeaponActor->TracingComponent->BoxHalfSize = FVector(CurrentTraceSize, CurrentTraceSize, CurrentTraceSize);
	}
	else
	{
		MeleeWeaponActor->TracingComponent->BoxHalfSize = FVector(TraceSize, TraceSize, TraceSize);
	}
	MeleeWeaponActor->TracingComponent->ToggleTraceCheck(true);
}

void UGA_MeleeWeaponAttack::EndMeleeWeaponTrace()
{
	MeleeWeaponActor->TracingComponent->ToggleTraceCheck(false);
	MeleeWeaponActor->TracingComponent->ClearHitArray();
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

void UGA_MeleeWeaponAttack::SelectHitReaction(UAbilitySystemComponent* TargetASC, UCentralStateComponent* CentralStateComponent,
	UCombatStateComponent* CombatStateComponent, FHitReactionStruct& InHitReactionStruct)
{
	TArray<FHitReactionStruct> TempPossibleHitReactions = PossibleHitReactions;

	if (TargetASC->HasMatchingGameplayTag(CentralStateComponent->GroundedTag))
	{
		for (int32 i = 0; i < TempPossibleHitReactions.Num(); i++)
		{
			if (TempPossibleHitReactions[i].CharacterOrientationTag.MatchesTagExact(CentralStateComponent->AirborneTag))
			{
				TempPossibleHitReactions.RemoveAt(i);
			}
		}
	}
	
	if (TargetASC->HasMatchingGameplayTag(CentralStateComponent->AirborneTag))
	{
		for (int32 i = 0; i < TempPossibleHitReactions.Num(); i++)
		{
			if (TempPossibleHitReactions[i].CharacterOrientationTag.MatchesTagExact(CentralStateComponent->GroundedTag))
			{
				TempPossibleHitReactions.RemoveAt(i);
			}
		}
	}
	
	if (TargetASC->HasMatchingGameplayTag(CombatStateComponent->NeutralTag))
	{
		for (int32 i = 0; i < TempPossibleHitReactions.Num(); i++)
		{
			//TODO: Fix later
			if (TempPossibleHitReactions[i].CharacterStateTags.HasTagExact(CombatStateComponent->DownedTag))
			{
				TempPossibleHitReactions.RemoveAt(i);
			}
		}
	}

	if (TargetASC->HasMatchingGameplayTag(CombatStateComponent->DownedTag))
	{
		for (int32 i = 0; i < TempPossibleHitReactions.Num(); i++)
		{
			//TODO: Fix later
			if (TempPossibleHitReactions[i].CharacterStateTags.HasTagExact(CombatStateComponent->NeutralTag))
			{
				TempPossibleHitReactions.RemoveAt(i);
			}
		}
	}
	if(TempPossibleHitReactions.Num() > 1)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SelectHitReaction - More than 2 possible hit reactions"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	if(TempPossibleHitReactions.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SelectHitReaction - No possible hit reactions"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	InHitReactionStruct = TempPossibleHitReactions[0];
}

bool UGA_MeleeWeaponAttack::GetTargetStateComponentsAndHitReaction(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitReactionStruct& InHitReactionStruct)
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
		UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent();
	
		if(TargetASC)
		{
			UCentralStateComponent* CentralStateComponent = HitActor->FindComponentByClass<UCentralStateComponent>();
			UCombatStateComponent* CombatStateComponent = HitActor->FindComponentByClass<UCombatStateComponent>();
	
			if(CentralStateComponent && CombatStateComponent)
			{
				SelectHitReaction(TargetASC, CentralStateComponent, CombatStateComponent, InHitReactionStruct);
				return true;
			}
		}
		
	}
	return false;
}

void UGA_MeleeWeaponAttack::ExecuteMeleeHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	FHitReactionStruct OutHitReactionStruct;
	bool FoundHitReaction = GetTargetStateComponentsAndHitReaction(TargetDataHandle, OutHitReactionStruct);

	if(!FoundHitReaction)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeWeaponAttack::ExecuteMeleeHitLogic - Hit reaction not found"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	ApplyMeleeHitEffects(TargetDataHandle, OutHitReactionStruct);
	SendMeleeHitGameplayEvents(TargetDataHandle, OutHitReactionStruct);
	AddMeleeHitCues(TargetDataHandle, OutHitReactionStruct);
	
}

void UGA_MeleeWeaponAttack::ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitReactionStruct CurrentHitReactionStruct)
{
	
	if(ApplyDamageEffect)
	{
		TArray<FActiveGameplayEffectHandle> AppliedDamageEffects = ApplyGameplayEffectToTarget(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		TargetDataHandle,
		ApplyDamageEffect, 
		1,
		1
		);
	}

	const FGameplayAbilityTargetData* HitActors = TargetDataHandle.Get(0);
	TArray<TWeakObjectPtr<AActor>> HitActor = HitActors->GetActors();
	AActor* EnemyActor = HitActor[0].Get();
	
	if (EnemyActor && EnemyActor->Implements<UAbilitySystemInterface>())
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(EnemyActor);
		UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent();

		if(TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Character.State.Downed")))
		{
			if(GrantDownedHitReactionEffect)
			{
				FGameplayEffectSpecHandle DownedHitEffectHandle = MakeOutgoingGameplayEffectSpec(GrantDownedHitReactionEffect, 1.0f);
			
				TArray<FActiveGameplayEffectHandle> TestAppliedHitEffects = ApplyGameplayEffectSpecToTarget(
						CurrentSpecHandle,
						CurrentActorInfo,
						CurrentActivationInfo,
						DownedHitEffectHandle,
						TargetDataHandle
					);
				
			}
		}
		else
		{
			if(CurrentHitReactionEffect)
			{
				TArray<FActiveGameplayEffectHandle> AppliedHitEffects = ApplyGameplayEffectToTarget(
				CurrentSpecHandle,
				CurrentActorInfo,
				CurrentActivationInfo,
				TargetDataHandle,
				CurrentHitReactionEffect, 
				1,
				1
				);
			}
			else
			{
				if(GrantHitReactionEffect)
				{
					FGameplayEffectSpecHandle HitEffectHandle = MakeOutgoingGameplayEffectSpec(GrantHitReactionEffect, 1.0f);
			
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
	}
	
}

void UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitReactionStruct CurrentHitReactionStruct)
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

	HitInfoObj->HitData.HitDirection = CurrentHitDirection;
	
	// if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CurrentHitReactionTag))
	// {
	// 	OnHitEventData.EventTag = CurrentHitReactionTag;
	// 	
	// }
	// else if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(HitReactionTag))
	// {
	// 	OnHitEventData.EventTag = HitReactionTag;
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - HitReactionTag is invalid"));
	// }

	const FGameplayAbilityTargetData* HitActors = TargetDataHandle.Get(0);
	TArray<TWeakObjectPtr<AActor>> HitActor = HitActors->GetActors();
	UE_LOG(LogTemp, Warning, TEXT("The name is: %s"), *HitActor[0]->GetName());
	
	AActor* EnemyActor = HitActor[0].Get();
	
	if (EnemyActor && EnemyActor->Implements<UAbilitySystemInterface>())
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(EnemyActor);
		UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent();

		if(TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Character.State.Downed")))
		{
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(DownedHitReactionTag))
			{
				OnHitEventData.EventTag = DownedHitReactionTag;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - HitReactionTag is invalid"));
			}
		}
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OnHitEventData.EventTag, OnHitEventData);
}

void UGA_MeleeWeaponAttack::AddMeleeHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitReactionStruct CurrentHitReactionStruct)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	
	FGameplayCueParameters HitCueParams;
	if(CurrentHitFX)
	{
		HitCueParams.SourceObject = static_cast<const UObject*>(CurrentHitFX);
	}
	else
	{
		HitCueParams.SourceObject = static_cast<const UObject*>(HitFX);
	}
	HitCueParams.EffectCauser = TargetActor;
	HitCueParams.Location = TargetDataHandle.Get(0)->GetHitResult()->Location;
		
	K2_AddGameplayCueWithParams(HitEffectCueTag, HitCueParams);
}

void UGA_MeleeWeaponAttack::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
	
}

void UGA_MeleeWeaponAttack::ExtractMeleeAssetProperties(TObjectPtr<UMeleeAbilityDataAsset> MeleeAsset)
{
	CurrentHitReactionEffect = nullptr;
	CurrentHitReactionTag = FGameplayTag::EmptyTag;
	CurrentHitFX = nullptr;
	CurrentSlashFX = nullptr;
	CurrentHitDirection = ESpatialDirection::None;
	CurrentTraceSize = 0.0f;
	
	if(MeleeAsset->GrantHitReactionEffect)
	{
		CurrentHitReactionEffect = MeleeAsset->GrantHitReactionEffect;
	}

	if(MeleeAsset->HitVFX)
	{
		CurrentHitFX = MeleeAsset->HitVFX;
	}

	if(MeleeAsset->SlashVFX)
	{
		CurrentSlashFX = MeleeAsset->SlashVFX;
	}

	if(MeleeAsset->TraceSize > 0.0f)
	{
		CurrentTraceSize = MeleeAsset->TraceSize;
	}

	if(MeleeAsset->HitDirection != ESpatialDirection::None)
	{
		CurrentHitDirection = MeleeAsset->HitDirection;
	}

	if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(MeleeAsset->HitReactionTag))
	{
		CurrentHitReactionTag = MeleeAsset->HitReactionTag;
	}
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
		if(CurrentSlashFX)
		{
			SlashCueParams.SourceObject = static_cast<const UObject*>(CurrentSlashFX);
		}
		else
		{
			SlashCueParams.SourceObject = static_cast<const UObject*>(SlashFX);
		}
		K2_AddGameplayCueWithParams(SlashEffectCueTag, SlashCueParams);
		StartMeleeWeaponTrace();
	}

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.EndSlash")))
	{
		K2_RemoveGameplayCue(SlashEffectCueTag);
		EndMeleeWeaponTrace();
	}
}
