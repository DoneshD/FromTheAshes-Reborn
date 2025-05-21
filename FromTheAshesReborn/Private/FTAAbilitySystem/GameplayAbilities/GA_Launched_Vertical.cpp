#include "FTAAbilitySystem/GameplayAbilities/GA_Launched_Vertical.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Launched_Vertical::UGA_Launched_Vertical()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Launched_Vertical::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if(IsLaunched)
	{
		UpdateLaunchedMovement(DeltaTime);
	}
}

bool UGA_Launched_Vertical::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !GetAbilitySystemComponentFromActorInfo())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Launched_Vertical::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	LaunchedElapsedTime = 0.0f;
	LaunchedStartTime = GetWorld()->GetTimeSeconds();

	LaunchedStartLocation = ActorInfo->AvatarActor->GetActorLocation();
	LaunchedEndLocation = ActorInfo->AvatarActor->GetActorLocation() + FVector(0.0f, 0.0f, LaunchedVerticalDistance);

	IsLaunched = true;

}

void UGA_Launched_Vertical::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Launched_Vertical::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Launched_Vertical::UpdateLaunchedMovement(float DeltaTime)
{
	LaunchedElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(LaunchedElapsedTime / LaunchedDuration, 0.0f, 1.0f);
	const FVector NewLocation = FMath::Lerp(LaunchedStartLocation, LaunchedEndLocation, Alpha);

	FHitResult Hit;
	CurrentActorInfo->AvatarActor->SetActorLocation(NewLocation, true, &Hit);

	if (Alpha >= 1.0f || Hit.bBlockingHit)
	{
		LaunchedLocationReached();
	}
}

void UGA_Launched_Vertical::LaunchedLocationReached()
{
	IsLaunched = false;
	
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity.Z = 0.0f;
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(AerialStallTimerHandle, this, &UGA_Launched_Vertical::EndAirStall, StallDuration, false);

	
}

void UGA_Launched_Vertical::EndAirStall()
{
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity.Z = -100.0f;
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 4.0f;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_Launched_Vertical::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_Launched_Vertical::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCompleted(EventTag, EventData);
}

void UGA_Launched_Vertical::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);
}
