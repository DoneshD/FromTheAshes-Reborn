#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_Slammed.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "EventObjects/SlamEventObject.h"
#include "FTAAbilitySystem/AbilityTasks/AT_SlamCharacterAndWait.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UGA_Slammed::UGA_Slammed()
{
	ReceiveHitTag = FGameplayTag::RequestGameplayTag("HitTag.Effect.GrantAbility.Slam");
}

void UGA_Slammed::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_Slammed::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Slammed::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const USlamEventObject* SlamInfoObject = Cast<USlamEventObject>(CurrentEventData.OptionalObject);
	
	if(!SlamInfoObject)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_Slammed::ActivateAbility - SlamInfoObject is Null"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	SlamTask = UAT_SlamCharacterAndWait::AT_SlamCharacterAndWait(this,
	SlamInfoObject->SlamData.Location,
	SlamInfoObject->SlamData.Speed,
	SlamInfoObject->SlamData.Duration);

	PossibleFollowupReactions = SlamInfoObject->HitData.PossibleFollowupReactions;

	HitLocation = SlamInfoObject->SlamData.Location;
	
	if (SlamTask)
	{
		SlamTask->OnSlamComplete.AddDynamic(this, &UGA_Slammed::OnSlamComplete);
		SlamTask->ReadyForActivation();
	}
}

void UGA_Slammed::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Slammed::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	GetFTACharacterFromActorInfo()->RemoveAerialEffects();
	
	FGameplayEventData FollowupEventData;
	
	FollowupEventData.Instigator = GetAvatarActorFromActorInfo();
	FollowupEventData.Target = GetAvatarActorFromActorInfo();
	
	UHitEventObject* HitInfoObj = NewObject<UHitEventObject>(this);
	HitInfoObj->HitData.Instigator = GetAvatarActorFromActorInfo();
	
	
	FollowupEventData.OptionalObject = HitInfoObj;
	
	if(PossibleFollowupReactions[0] && PossibleFollowupReactions[0]->IsValidLowLevel() && PossibleFollowupReactions.Num() > 0 && !PossibleFollowupReactions.IsEmpty())
	{
		const UGA_ReceiveHit* const CDO = PossibleFollowupReactions[0]->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->ReceiveHitTag))
			{
				const TSubclassOf<UGameplayEffect>* GrantAbilityEffect = CDO->ReceiveHitEffectMap.Find(CDO->ReceiveHitTag);

				if(GrantAbilityEffect)
				{
					FGameplayEffectSpecHandle GrantAbilityEffectHandle = MakeOutgoingGameplayEffectSpec(*GrantAbilityEffect, 1.0f);

					FActiveGameplayEffectHandle AppliedEffects = ApplyGameplayEffectSpecToOwner(
						CurrentSpecHandle,
						CurrentActorInfo,
						CurrentActivationInfo,
						GrantAbilityEffectHandle
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
			
			
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->ReceiveHitTag))
			{
				FollowupEventData.EventTag = CDO->ReceiveHitTag;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_Slammed::EndAbility - FollowupEventData is NULL"));
			}
			
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), FollowupEventData.EventTag, FollowupEventData);
		}
		
	}
	
}

void UGA_Slammed::OnSlamComplete()
{

	if (ImpactNiagara)
	{
		float RandomPitch = FMath::FRandRange(-30.f, 30.f);
		FRotator RandomRotation(RandomPitch, 0.f, 0.f);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactNiagara,
			HitLocation,
			RandomRotation,
			FVector(1.f),
			true,  
			true,  
			ENCPoolMethod::None,
			true);

	}

	if (ImpactSound)
	{
		float RandomPitch = UKismetMathLibrary::RandomFloatInRange(0.90, 1.30f);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, HitLocation, .2, RandomPitch, .1);
	}

	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
