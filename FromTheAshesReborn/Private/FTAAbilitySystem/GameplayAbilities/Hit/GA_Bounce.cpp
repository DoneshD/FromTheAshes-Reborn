
#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_Bounce.h"

#include "AbilitySystemComponent.h"
#include "CombatComponents/AerialCombatComponent.h"
#include "CombatComponents/CentralStateComponent.h"
#include "CombatComponents/DownedCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"

UGA_Bounce::UGA_Bounce()
{
	ReceiveHitTag = FGameplayTag::RequestGameplayTag("HitTag.Effect.GrantAbility.Bounce");
	
}

void UGA_Bounce::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if(TraceForFloor)
	{
		CheckGroundBelow();
	}
}

bool UGA_Bounce::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Bounce::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAerialCombatComponent* ACC = GetFTACharacterFromActorInfo()->FindComponentByClass<UAerialCombatComponent>();
	
	if(ACC && ACC->IsValidLowLevel())
	{
		
		if(ACC->EnableAerialCombatEffect)
		{
			UCentralStateComponent* CSC = GetFTACharacterFromActorInfo()->FindComponentByClass<UCentralStateComponent>();
			if(CSC)
			{
				CSC->SetCurrentOrientation(CSC->AirborneOrientationTag, MOVE_Flying);
			}
			FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(ACC->EnableAerialCombatEffect, 1.0f);
			GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
			
		}
	}
	

}

void UGA_Bounce::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Bounce::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}


void UGA_Bounce::CheckGroundBelow()
{
	FHitResult Hit;
	
	UCapsuleComponent* Capsule = GetFTACharacterFromActorInfo()->GetCapsuleComponent();
	
	FVector Center = Capsule->GetComponentLocation();
	float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	float Radius = Capsule->GetScaledCapsuleRadius();

	FVector End = Center + FVector(0.f, 0.f, -1.f) * 20.0f;
	
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetFTACharacterFromActorInfo());
	
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Center,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeCapsule(Radius, HalfHeight),
		Params
	);
	
	if (bHit)
	{
		TraceForFloor = false;

		UCentralStateComponent* CSC = GetFTACharacterFromActorInfo()->FindComponentByClass<UCentralStateComponent>();
		if(CSC)
		{
			CSC->SetCurrentOrientation(CSC->GroundedOrientationTag, MOVE_Walking);
		}

		UDownedCombatComponent* DCC = GetFTACharacterFromActorInfo()->FindComponentByClass<UDownedCombatComponent>();
	
		if(DCC->EnableDownedCombatEffect)
		{
			FGameplayEffectSpecHandle GEHandle = MakeOutgoingGameplayEffectSpec(DCC->EnableDownedCombatEffect, 1.0f);
			GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
		}
		
	}
	
	DrawDebugCapsule(
		GetWorld(),
		Center,
		HalfHeight,
		Radius,
		FQuat::Identity,
		bHit ? FColor::Green : FColor::Red,
		false,
		0.1f
	);
}


void UGA_Bounce::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_Bounce::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
}

void UGA_Bounce::OnMontageBlendingOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageBlendingOut(EventTag, EventData);
	

}

void UGA_Bounce::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);

	if(!UTagValidationFunctionLibrary::IsRegisteredGameplayTag(EventTag))
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_Bounce::EventMontageReceived - EventTag is invalid"))
		return;
	}
	
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("TestTag.Tag7")))
	{
		TraceForFloor = true;
	}
	
}
