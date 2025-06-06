#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_Aerial.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ComboManagerComponent.h"
#include "EventObjects/SuspendEventObject.h"
#include "FTAAbilitySystem/AbilityTasks/AT_SuspendInAirAndWait.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"

UGA_MeleeWeaponAttack_Aerial::UGA_MeleeWeaponAttack_Aerial()
{
}

void UGA_MeleeWeaponAttack_Aerial::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_MeleeWeaponAttack_Aerial::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
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

	return Character->GetCharacterMovement()->IsFalling() || Character->GetCharacterMovement()->IsFlying();
}

void UGA_MeleeWeaponAttack_Aerial::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ComboManagerComponent->AerialAttacksCounter += 1;

	SuspendTask = UAT_SuspendInAirAndWait::AT_SuspendInAirAndWait(this,
		DescentSpeed,
		5.0f);

	SuspendTask->ReadyForActivation();
	
}

void UGA_MeleeWeaponAttack_Aerial::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeWeaponAttack_Aerial::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	//TODO: This is being called from a grounded attack, check later 
}

void UGA_MeleeWeaponAttack_Aerial::SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	USuspendEventObject* SuspendInfoObj = NewObject<USuspendEventObject>(this);
	SuspendInfoObj->SuspendData.DescentSpeed = DescentSpeed;

	FGameplayEventData OnAerialHitEventData;
	OnAerialHitEventData.OptionalObject = SuspendInfoObj;

	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	
	OnAerialHitEventData.Instigator = GetAvatarActorFromActorInfo();
	OnAerialHitEventData.Target = TargetActor;
	OnAerialHitEventData.ContextHandle.AddHitResult(*TargetDataHandle.Get(0)->GetHitResult());
	
	if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(FGameplayTag::RequestGameplayTag("AbilityTag.Suspend")))
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilityTag.Suspend"));
		OnAerialHitEventData.EventTag = FGameplayTag::RequestGameplayTag("AbilityTag.Suspend");
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_MeleeWeaponAttack - AbilityTag.Suspend is NULL"));
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OnAerialHitEventData.EventTag, OnAerialHitEventData);
	
	// Super::SendMeleeHitGameplayEvents(TargetDataHandle);
}


void UGA_MeleeWeaponAttack_Aerial::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeWeaponAttack_Aerial::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);

	if(SuspendTask)
	{
		SuspendTask->EndTask();
	}
}

void UGA_MeleeWeaponAttack_Aerial::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
