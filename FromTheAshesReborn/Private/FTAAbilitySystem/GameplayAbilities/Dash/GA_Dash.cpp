#include "FTAAbilitySystem/GameplayAbilities/Dash/GA_Dash.h"

#include "ComboManagerComponent.h"
#include "FTAAbilitySystem/AbilityTasks/FTAAT_OnTick.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"

UGA_Dash::UGA_Dash()
{
	
}

void UGA_Dash::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if(IsDashing)
	{
		UpdateDashMovement(DeltaTime);
	}
}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	DashElapsedTime = 0.0f;
	DashStartTime = GetWorld()->GetTimeSeconds();

	FVector InputDir = UInputReadingFunctionLibrary::CheckInputVector(GetFTACharacterFromActorInfo()->GetCharacterMovement());

	DashStartLocation = ActorInfo->AvatarActor->GetActorLocation();
	DashEndLocation = ActorInfo->AvatarActor->GetActorLocation() + (InputDir * DashDistance);

	IsDashing = true;
	
	DrawDebugPoint(GetWorld(), DashEndLocation, 10.f, FColor::Red, false, 1.0f);
}

bool UGA_Dash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Dash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

void UGA_Dash::UpdateDashMovement(float DeltaTime)
{
	DashElapsedTime += DeltaTime;
		
	float Alpha = FMath::Clamp(DashElapsedTime / DashDuration, 0.0f, 1.0f);
	FVector NewLocation = FMath::Lerp(DashStartLocation, DashEndLocation, Alpha);

	FHitResult Hit;
	CurrentActorInfo->AvatarActor->SetActorLocation(NewLocation, true, &Hit);

	if (Alpha >= 1.0f || Hit.bBlockingHit)
	{
		DashLocationReached();
	}
}

void UGA_Dash::DashLocationReached()
{
	
}
