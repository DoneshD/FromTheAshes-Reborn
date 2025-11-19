#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_Launched.h"

#include "CombatComponents/AerialCombatComponent.h"
#include "EventObjects/LaunchEventObject.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/AT_LaunchCharacterAndWait.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack.h"
#include "FTACustomBase/FTACharacter.h"
#include "Kismet/GameplayStatics.h"

UGA_Launched::UGA_Launched()
{
	ReceiveHitTag = FGameplayTag::RequestGameplayTag("HitTag.Effect.GrantAbility.Launch");
}

void UGA_Launched::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_Launched::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Launched::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ULaunchEventObject* LaunchInfoObject = Cast<ULaunchEventObject>(CurrentEventData.OptionalObject);

	
	if(!LaunchInfoObject)
	{
		float ZDelta = (UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation().Z - GetFTACharacterFromActorInfo()->GetActorLocation().Z) - 300.0f;
		
		LaunchInfoObject = NewObject<ULaunchEventObject>(this);
		LaunchInfoObject->LaunchData.VerticalDistance = ZDelta;
		LaunchInfoObject->LaunchData.LaunchDuration = 0.10f;
		LaunchInfoObject->LaunchData.StallDuration = 0.2;
		LaunchInfoObject->LaunchData.Offset = LaunchOffset;

	
	}
	
	LaunchTask = UAT_LaunchCharacterAndWait::AT_LaunchCharacterAndWait(
		this,
		LaunchInfoObject->LaunchData.VerticalDistance,
		LaunchInfoObject->LaunchData.LaunchDuration,
		LaunchInfoObject->LaunchData.StallDuration,
		LaunchOffset);
	
	if (LaunchTask)
	{
		LaunchTask->OnLaunchComplete.AddDynamic(this, &UGA_Launched::OnLaunchComplete);
		LaunchTask->ReadyForActivation();
	}
	
}

void UGA_Launched::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Launched::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("HitTag.Effect.Flail"));

	UAerialCombatComponent* AerialCombatComponent = GetFTACharacterFromActorInfo()->FindComponentByClass<UAerialCombatComponent>();

	if(AerialCombatComponent && AerialCombatComponent->IsValidLowLevel())
	{
		if(AerialCombatComponent->EnableAerialCombatEffect)
		{
			FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(AerialCombatComponent->EnableAerialCombatEffect, 1.0f);
			GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid pointer or null"))
	}
	
	if (GetFTAAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(ReceiveHitTag))
	{
		GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(ReceiveHitTag));
	}
	
}

void UGA_Launched::OnLaunchComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

}
