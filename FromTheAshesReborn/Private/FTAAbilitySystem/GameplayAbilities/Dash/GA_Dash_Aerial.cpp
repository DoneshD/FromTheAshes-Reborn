#include "FTAAbilitySystem/GameplayAbilities/Dash/GA_Dash_Aerial.h"

UGA_Dash_Aerial::UGA_Dash_Aerial()
{
	
}

void UGA_Dash_Aerial::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if(IsGliding)
	{
		UpdateGlideMovement(DeltaTime);
	}
	
}


void UGA_Dash_Aerial::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	GlideElapsedTime = 0.0f;
	IsGliding = false;
}

bool UGA_Dash_Aerial::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Dash_Aerial::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Dash_Aerial::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Dash_Aerial::UpdateGlideMovement(float DeltaTime)
{
	GlideElapsedTime += DeltaTime;

	FVector CurrentLocation = CurrentActorInfo->AvatarActor->GetActorLocation();
	FVector NewLocation = CurrentLocation + GlideVelocity * DeltaTime;

	DrawDebugLine(GetWorld(), CurrentLocation, NewLocation, FColor::Green, true, 1.0f, 0, 2.0f);
	DrawDebugPoint(GetWorld(), NewLocation, 10.f, FColor::Red, true, 1.0f);

	GlideVelocity += FVector(0.f, 0.f, -980.f) * DeltaTime * GlideGravityScale;
	GlideVelocity *= GlideDrag;

	FHitResult Hit;
	CurrentActorInfo->AvatarActor->SetActorLocation(NewLocation, true, &Hit);

	//Add check for isFalling
	if (GlideElapsedTime >= GlideTimeDuration || Hit.bBlockingHit)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	}

}

void UGA_Dash_Aerial::DashLocationReached()
{
	Super::DashLocationReached();
	
	FVector DashDirection = (DashEndLocation - DashStartLocation).GetSafeNormal();
	GlideVelocity = DashDirection * InitialForwardGlideSpeed + FVector(0.f, 0.f, InitialDownwardVelocity);

	GlideElapsedTime = 0.0f;
	
	IsDashing = false;
	IsGliding = true;
	
}
