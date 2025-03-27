#include "FTAAbilitySystem/GameplayAbilities/Dash/GA_Dash.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_OnTick.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"

UGA_Dash::UGA_Dash()
{
	
}

void UGA_Dash::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if (!IsGliding)
	{
		DashElapsedTime += DeltaTime;
		float Alpha = FMath::Clamp(DashElapsedTime / DashDuration, 0.0f, 1.0f);
		FVector NewLocation = FMath::Lerp(DashStartLocation, DashEndLocation, Alpha);

		FHitResult Hit;
		CurrentActorInfo->AvatarActor->SetActorLocation(NewLocation, true, &Hit);

		if (Alpha >= 1.0f || Hit.bBlockingHit)
		{
			IsGliding = true;
			GlideElapsedTime = 0.0f;

			FVector DashDirection = (DashEndLocation - DashStartLocation).GetSafeNormal();
			GlideVelocity = DashDirection * ForwardGlideSpeed + FVector(0.f, 0.f, InitialDownwardVelocity);
		}
	}
	else
	{
		GlideElapsedTime += DeltaTime;

		FVector CurrentLocation = CurrentActorInfo->AvatarActor->GetActorLocation();
		FVector NewLocation = CurrentLocation + GlideVelocity * DeltaTime;

		DrawDebugLine(GetWorld(), CurrentLocation, NewLocation, FColor::Green, false, 1.0f, 0, 2.0f);
		DrawDebugPoint(GetWorld(), NewLocation, 10.f, FColor::Red, false, 1.0f);

		GlideVelocity += FVector(0.f, 0.f, -980.f) * DeltaTime * GlideGravityScale;
		GlideVelocity *= GlideDrag;

		FHitResult Hit;
		CurrentActorInfo->AvatarActor->SetActorLocation(NewLocation, true, &Hit);

		if (GlideElapsedTime >= GlideTimeDuration || Hit.bBlockingHit)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		}
	}

}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo,
                               const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	IsGliding = false;
	DashElapsedTime = 0.0f;
	GlideElapsedTime = 0.0f;

	FVector InputDir = UInputReadingFunctionLibrary::CheckInputVector(GetFTACharacterFromActorInfo()->GetCharacterMovement());
	FVector ActorLocation = ActorInfo->AvatarActor->GetActorLocation();

	DashStartLocation = ActorLocation;
	DashEndLocation = ActorLocation + (InputDir * DashDistance);
	DrawDebugPoint(GetWorld(), DashEndLocation, 10.f, FColor::Red, false, 1.0f);

	DashStartTime = GetWorld()->GetTimeSeconds();
}

bool UGA_Dash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
                                  FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Dash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                             const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                          bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
