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
#include "Camera/CameraSystemComponent.h"
#include "Camera/CameraSystemParams.h"
#include "NiagaraSystem.h"
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
		return;
	}
	
	ComboManagerComponent = FTAChar->ComboManagerComponent;

	if(!ComboManagerComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - ComboManagerComponent is Null"));
		return;
	}

	if(!FTAChar->EquipmentManagerComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - EquipmentManagerComponent is Null"));
		return;
	}
	
	MeleeWeaponActor = FTAChar->EquipmentManagerComponent->GetEquippedWeaponActor();
	
	if(!MeleeWeaponActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::ActivateAbility - MeleeWeaponActor is Null"));
		return;
	}
	
	MeleeWeaponActor->TracingComponent->OnItemAdded.AddDynamic(this, &UGA_MeleeWeaponAttack::OnHitAdded);
	MeleeWeaponActor->TracingComponent->BoxHalfSize = FVector(BoxHalfSize, BoxHalfSize, BoxHalfSize);
	
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
	MeleeWeaponActor->TracingComponent->BoxHalfSize = FVector(20.0f, 20.0f, 20.0f);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (!UTagValidationFunctionLibrary::IsRegisteredGameplayTag(StateTreeFinishedTag))
		{
			return;
		}

		ASC->AddLooseGameplayTag(StateTreeFinishedTag);
		ASC->RemoveLooseGameplayTag(StateTreeFinishedTag);
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

	ExtractMeleeAssetProperties(MatchingDataAsset);
	
	if (MatchingDataAsset->MontageToPlay)
	{
		int32 CurrentComboIndex = ComboManagerComponent->GetCurrentComboIndex();
		
		ComboManagerComponent->GetCurrentComboContainer().AddTag(MatchingDataAsset->UniqueIdentifierTag);
		ComboManagerComponent->SetCurrentComboIndex(CurrentComboIndex + 1);
		ComboManagerComponent->PauseCurrentAttack = false;

		PlayAbilityAnimMontage(MatchingDataAsset->MontageToPlay);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AttackMontageToPlay is NULL"));
		ResetMeleeAttack();
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

void UGA_MeleeWeaponAttack::OnHitAdded(FHitResult LastItem)
{
	AActor* TargetActor = LastItem.GetActor();

	if (TargetActor && TargetActor->Implements<UAbilitySystemInterface>())
	{
		IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(TargetActor);
		UAbilitySystemComponent* TargetASC = AbilitySystemInterface->GetAbilitySystemComponent();

		//TODO: Temporary, change later
		AEnemyBaseCharacter* Enemy = Cast<AEnemyBaseCharacter>(Cast<AEnemyBaseCharacter>(GetFTACharacterFromActorInfo()));
		AEnemyBaseCharacter* AIEnemy = Cast<AEnemyBaseCharacter>(Cast<AEnemyBaseCharacter>(TargetActor));

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

void UGA_MeleeWeaponAttack::ExecuteMeleeHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	ApplyMeleeHitEffects(TargetDataHandle);
	SendMeleeHitGameplayEvents(TargetDataHandle);
	AddMeleeHitCues(TargetDataHandle);
}

void UGA_MeleeWeaponAttack::ApplyMeleeHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	
	if(DamageEffect)
	{
		TArray<FActiveGameplayEffectHandle> AppliedDamageEffects = ApplyGameplayEffectToTarget(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		TargetDataHandle,
		DamageEffect, 
		1,
		1
		);
	}
	if (!GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("TestTag.Tag13")))
	{
		if(CurrentHitReactionEffect)
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentHitReactionEffect valid"));
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
			if(HitReactionEffect)
			{
				UE_LOG(LogTemp, Warning, TEXT("HitReactionEffect valid"));
				
				// TArray<FActiveGameplayEffectHandle> AppliedHitEffects = ApplyGameplayEffectToTarget(
				// CurrentSpecHandle,
				// CurrentActorInfo,
				// CurrentActivationInfo,
				// TargetDataHandle,
				// HitReactionEffect, 
				// 1,
				// 1
				// );

				FGameplayEffectSpecHandle TestHandle = MakeOutgoingGameplayEffectSpec(HitReactionEffect, 1.0f);
				
				TArray<FActiveGameplayEffectHandle> TestAppliedHitEffects = ApplyGameplayEffectSpecToTarget(
						CurrentSpecHandle,
						CurrentActorInfo,
						CurrentActivationInfo,
						TestHandle,
						TargetDataHandle
					);

				if (GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("TestTag.Tag13")))
				{
					GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("TestTag.Tag13")));
					UE_LOG(LogTemp, Warning, TEXT("Removing!"));
		
				}
			}
			
		}
	}
	
	
}

void UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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
	
	// HitInfoObj->HitData.HitDirection = MeleeWeaponActor->HitDirection;
	HitInfoObj->HitData.HitDirection = TestHitDirection;
	
	OnHitEventData.OptionalObject = HitInfoObj;
	
	if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CurrentHitReactionTag))
	{
		OnHitEventData.EventTag = CurrentHitReactionTag;
		
	}
	else if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(HitReactionTag))
	{
		OnHitEventData.EventTag = HitReactionTag;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - HitReactionTag is invalid"));
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OnHitEventData.EventTag, OnHitEventData);
}

void UGA_MeleeWeaponAttack::AddMeleeHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
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
		
	K2_AddGameplayCueWithParams(HitVfxCueTag, HitCueParams);
}

void UGA_MeleeWeaponAttack::PlayAbilityAnimMontage(TObjectPtr<UAnimMontage> AnimMontage)
{
	Super::PlayAbilityAnimMontage(AnimMontage);
	
}

void UGA_MeleeWeaponAttack::ExtractMeleeAssetProperties(TObjectPtr<UMeleeAbilityDataAsset> MeleeAsset)
{
	CurrentHitReactionEffect = nullptr;
	CurrentHitFX = nullptr;
	CurrentSlashFX = nullptr;
	CurrentHitReactionTag = FGameplayTag::EmptyTag;
	
	if(MeleeAsset->HitEffect)
	{
		CurrentHitReactionEffect = MeleeAsset->HitEffect;
	}

	if(MeleeAsset->HitVFxImpact)
	{
		CurrentHitFX = MeleeAsset->HitVFxImpact;
	}

	if(MeleeAsset->SlashFX)
	{
		CurrentSlashFX = MeleeAsset->SlashFX;
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
		K2_AddGameplayCueWithParams(SlashVfxCueTag, SlashCueParams);
	}

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.EndSlash")))
	{
		K2_RemoveGameplayCue(SlashVfxCueTag);
	}
}
