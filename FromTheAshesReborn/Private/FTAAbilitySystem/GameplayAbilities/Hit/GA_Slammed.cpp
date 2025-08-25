#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_Slammed.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EventObjects/SlamEventObject.h"
#include "FTAAbilitySystem/AbilityTasks/AT_SlamCharacterAndWait.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"

UGA_Slammed::UGA_Slammed()
{
	
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
	
	FGameplayEventData RecoverEventData;
	
	RecoverEventData.Instigator = GetAvatarActorFromActorInfo();
	RecoverEventData.Target = GetAvatarActorFromActorInfo();

	UHitEventObject* HitInfoObj = NewObject<UHitEventObject>(this);
	HitInfoObj->HitData.Instigator = GetAvatarActorFromActorInfo();
	
	
	RecoverEventData.OptionalObject = HitInfoObj;
	
	if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.GrantAbility.Knockdown")))
	{
		RecoverEventData.EventTag = FGameplayTag::RequestGameplayTag("HitTag.Effect.GrantAbility.Knockdown");
		UE_LOG(LogTemp, Warning, TEXT("Grant Knockdown"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_KnockDown::OnMontageBlendingOut - RecoveryTag is NULL"));
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), RecoverEventData.EventTag, RecoverEventData);
	
}

void UGA_Slammed::OnSlamComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
