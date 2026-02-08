#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"

#include "CombatComponents/ComboManagerComponent.h"
#include "DataAsset/HitReactionDataAsset.h"
#include "Enemy/AIControllerEnemyBase.h"
#include "EventObjects/HitEventObject.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UGA_ReceiveHit::UGA_ReceiveHit()
{
	HitTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Stagger")));
	HitTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Suspend")));
	HitTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Launch")));
	HitTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Knockdown")));
	HitTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Knockback")));
	HitTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Slam")));
	HitTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.DownedFlinch")));
	HitTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Bounce")));

}
void UGA_ReceiveHit::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
	
}

bool UGA_ReceiveHit::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_ReceiveHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	for (const FGameplayTag& Tag : HitTagContainer)
	{
		if (!Tag.MatchesTagExact(ReceiveHitTag))
		{
			GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(Tag));
		}
	}
	
	if(GetFTACharacterFromActorInfo()->IsDead || GetFTACharacterFromActorInfo()->IsAlreadyDead)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	if(!CurrentEventData.OptionalObject)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_ReceiveHit::ActivateAbility - CurrentEventData.OptionalObject is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	if(!GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.State.Hit")))
	{
		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.State.Hit"));
	}
	
	if (AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(GetControllerFromActorInfo()))
	{
		FGameplayTag HitTag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.Hit");
		
		const UStateTreeComponent* STComp = EnemyController->StateTreeComponent;

		if (STComp)
		{
			FStateTreeEvent HitEvent;
			HitEvent.Tag = HitTag;
			EnemyController->StateTreeComponent->SendStateTreeEvent(HitEvent);
		}
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("No enemy controller"));
	}
	
	/*if(!NonMontageAbility)
	{
		TArray<UHitReactionDataAsset*> AssetsToTry;
		for (UHitReactionDataAsset* Asset : HitAbilityAssets)
		{
			if(Asset->Direction == HitInfoObject->HitData.HitDirection || Asset->Direction == ESpatialDirection::Any)
			{
				if(Asset->MontageToPlay)
				{
					AssetsToTry.Add(Asset);
				}
			}
		}
		
		if(AssetsToTry.Num() > 0)
		{
			ESpatialDirection Dir = HitInfoObject->HitData.HitDirection;

			// UE_LOG(LogTemp, Warning, TEXT("HitDirection: %s"),
			// 	*StaticEnum<ESpatialDirection>()->GetNameStringByValue((int64)Dir));
			
			int Selection = FMath::RandRange(0, AssetsToTry.Num() - 1);
			if(HitInfoObject->HitData.HitDirection == ESpatialDirection::Knockback)
			{
				Selection = 0;
				if(GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent")))
				{
					Selection = 1;
				}
			}
			// PlayAbilityAnimMontage(AssetsToTry[Selection]->MontageToPlay);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UGA_ReceiveHit::ActivateAbility - No possible assets"));
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
			return;
		}
	}*/
}

void UGA_ReceiveHit::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

}

void UGA_ReceiveHit::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if(!GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StateTreeTag.Status.Ability.Hit.Finished")))
	{
		GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("StateTreeTag.Status.Ability.Hit.Finished"));
		GetFTAAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("StateTreeTag.Status.Ability.Hit.Finished"));
	}

	if(GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.State.Hit")))
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.State.Hit"));
	}
	HitInfoObject = nullptr;

}

void UGA_ReceiveHit::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
	
}

void UGA_ReceiveHit::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

}

void UGA_ReceiveHit::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}

void UGA_ReceiveHit::PerformAbility(UFTAAbilityDataAsset* InAbilityAsset)
{
	Super::PerformAbility(InAbilityAsset);
	UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *InAbilityAsset->GetName());

	UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), HitInfoObject->HitData.Distance);
	
}

UFTAAbilityDataAsset* UGA_ReceiveHit::SelectAbilityAsset(TArray<UFTAAbilityDataAsset*> InAbilityAssets)
{
	Super::SelectAbilityAsset(InAbilityAssets);

	HitInfoObject = Cast<UHitEventObject>(CurrentEventData.OptionalObject);
	
	if(!HitInfoObject)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_ReceiveHit::ActivateAbility - HitInfoObject is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return nullptr;
	}
	
	FVector TargetLocation;
	
	if(!HitInfoObject->HitData.Instigator)
	{
		TargetLocation = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	}
	else
	{
		TargetLocation = HitInfoObject->HitData.Instigator->GetActorLocation();
	}
	
	FVector StartLocation = GetFTACharacterFromActorInfo()->GetActorLocation(); 
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
	GetFTACharacterFromActorInfo()->SetActorRotation(FRotator(0, LookAtRotation.Yaw, 0));

	TArray<UHitReactionDataAsset*> HitDataAssets;

	for (UFTAAbilityDataAsset* Asset : InAbilityAssets)
	{
		if (UHitReactionDataAsset* HitAsset = Cast<UHitReactionDataAsset>(Asset))
		{
			HitDataAssets.Add(HitAsset);
		}
	}

	if(TObjectPtr<UHitReactionDataAsset> AbilityDataAsset = ComboManagerComponent->GetHitAssetByRequirements(HitDataAssets, HitInfoObject))
	{
		return AbilityDataAsset;
	}
	return nullptr;

	
	
}
