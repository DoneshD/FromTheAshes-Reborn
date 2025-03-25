#include "FTAAbilitySystem/GameplayAbilities/Dash/GA_Dash.h"
#include "MotionWarpingComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/FTAPlayerState.h"

class AFTAPlayerState;

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

	FVector TargetLocation = UInputReadingFunctionLibrary::CheckInputVector(GetFTACharacterFromActorInfo()->GetCharacterMovement());
	DashTargetLocation = ActorInfo->AvatarActor->GetActorLocation() + DashWarpDistance * TargetLocation;

	//TODO: Player only, fix later for enemies
	FRotator TargetRotation;
	if(GetFTAPlayerStateFromOwnerInfo())
	{
		TargetRotation = ULockOnFunctionLibrary::CheckRotationBasedOnTarget(CurrentActorInfo->AvatarActor.Get(), GetFTAPlayerStateFromOwnerInfo()->HardLockedTargetActor, DashTargetLocation);
	}
	
	GetFTACharacterFromActorInfo()->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(FName("DashTarget"), DashTargetLocation, TargetRotation);
		
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
