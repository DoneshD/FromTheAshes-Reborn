#include "FTAAbilitySystem/GameplayAbilities/GA_AerialHit_Suspended.h"

#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_AerialHit_Suspended::UGA_AerialHit_Suspended()
{
}

void UGA_AerialHit_Suspended::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if(bDescend)
	{
		UpdateAerialDescentMovement(DeltaTime);
	}
}

bool UGA_AerialHit_Suspended::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_AerialHit_Suspended::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity.Z = 0.0f;
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 0.0f;
	bDescend = true;

	GetWorld()->GetTimerManager().SetTimer(AerialSuspensionTimer, this, &UGA_AerialHit_Suspended::EndAirStall, SuspendTimerSpeed, false);
	
}

void UGA_AerialHit_Suspended::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_AerialHit_Suspended::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_AerialHit_Suspended::UpdateAerialDescentMovement(float DeltaTime)
{
	FHitResult Hit;
	
	FVector CurrentLocation = GetFTACharacterFromActorInfo()->GetActorLocation();
	// FVector MoveDelta = FVector(0.0f, 0.0f, -1.0f).GetSafeNormal() * (DescentSpeed * FMath::Square(ComboManagerComponent->AerialAttacksCounter)) * DeltaTime;
	FVector MoveDelta = FVector(0.0f, 0.0f, -1.0f).GetSafeNormal() * DescentSpeed * DeltaTime;
	
	FVector NewLocation = CurrentLocation + MoveDelta;

	GetFTACharacterFromActorInfo()->SetActorLocation(NewLocation, true, &Hit);

	if (Hit.bBlockingHit)
	{
		EndAirStall();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	}
}

void UGA_AerialHit_Suspended::EndAirStall()
{
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity.Z = -100.0f;
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 4.0f;
	bDescend = false;
}

void UGA_AerialHit_Suspended::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_AerialHit_Suspended::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
	EndAirStall();
}

void UGA_AerialHit_Suspended::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
