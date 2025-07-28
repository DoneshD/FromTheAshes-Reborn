#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"

#include "DataAsset/HitReactionDataAsset.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "EventObjects/HitEventObject.h"

#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "Kismet/KismetMathLibrary.h"

UGA_ReceiveHit::UGA_ReceiveHit()
{
	HitAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Stagger")));
	HitAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Suspend")));
	HitAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Launch")));
	HitAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Knockdown")));
	HitAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Knockback")));
	HitAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("HitTag.Effect.GrantAbility.Slam")));
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

	for (const FGameplayTag& Tag : HitAbilityTagContainer)
	{
		if (!Tag.MatchesTagExact(HitReactionTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("Canceling tag: %s"), *Tag.ToString());
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
	
	const UHitEventObject* HitInfoObject = Cast<UHitEventObject>(CurrentEventData.OptionalObject);
	
	if(!HitInfoObject)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_ReceiveHit::ActivateAbility - HitInfoObject is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	if(!HitInfoObject->HitData.Instigator)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_ReceiveHit::ActivateAbility - HitInfoObject is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	FVector StartLocation = GetFTACharacterFromActorInfo()->GetActorLocation(); 
	FVector TargetLocation = HitInfoObject->HitData.Instigator->GetActorLocation();
	
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
	
	GetFTACharacterFromActorInfo()->SetActorRotation(FRotator(0, LookAtRotation.Yaw, 0));

	
	if(!NonMontageAbility)
	{
		TArray<UHitReactionDataAsset*> AssetsToTry;
		for (UHitReactionDataAsset* Asset : HitAbilityAssets)
		{
			if(Asset->HitReactionDirection == HitInfoObject->HitData.HitDirection)
			{
				if(Asset->MontageToPlay)
				{
					AssetsToTry.Add(Asset);
				}
			}
		}
		
		if(AssetsToTry.Num() > 0)
		{
			int Selection = FMath::RandRange(0, AssetsToTry.Num() - 1);
			PlayAbilityAnimMontage(AssetsToTry[Selection]->MontageToPlay);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UGA_ReceiveHit::ActivateAbility - No possible assets"));
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
			return;
		}
	}
}

void UGA_ReceiveHit::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

}

void UGA_ReceiveHit::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if(!bCanceled)
	{
		GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(HitReactionTag));
	}

}

void UGA_ReceiveHit::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);

	UE_LOG(LogTemp, Warning, TEXT("Cancel"));

	bCanceled = true;
	
	
}

void UGA_ReceiveHit::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

	UE_LOG(LogTemp, Warning, TEXT("End"));
	

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

}

void UGA_ReceiveHit::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
