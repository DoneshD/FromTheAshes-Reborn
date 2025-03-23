#include "FTAAbilitySystem/GameplayAbilities/GA_Dash.h"
#include "MotionWarpingComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "Kismet/KismetMathLibrary.h"

UGA_Dash::UGA_Dash()
{
	
}

void UGA_Dash::AbilityTickComponent()
{
	Super::AbilityTickComponent();
	
}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	DashTargetLocation = ActorInfo->AvatarActor->GetActorLocation() + 800.0f * ActorInfo->AvatarActor->GetActorForwardVector();
	
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetFTACharacterFromActorInfo()->GetActorLocation(), DashTargetLocation);
	GetFTACharacterFromActorInfo()->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(FName("DashTarget"),
		DashTargetLocation,
		Rotation
		);
		
}

bool UGA_Dash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Dash::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
