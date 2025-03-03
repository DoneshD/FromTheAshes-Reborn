#include "FTAAbilitySystem/GameplayAbilities/GA_Dash_Grounded.h"
#include "MotionWarpingComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "Kismet/KismetMathLibrary.h"

void UGA_Dash_Grounded::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FVector DashTargetLocation = ActorInfo->AvatarActor->GetActorLocation() + 2000.0f * ActorInfo->AvatarActor->GetActorForwardVector();
	DrawDebugSphere(GetWorld(), DashTargetLocation, 100, 12, FColor::Red, true);

	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetFTACharacterFromActorInfo()->GetActorLocation(), DashTargetLocation);
	GetFTACharacterFromActorInfo()->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(FName("DashTarget"),
		DashTargetLocation,
		Rotation
		);

	UE_LOG(LogTemp, Warning, TEXT("UGA_Dash_Grounded Here"))
}

bool UGA_Dash_Grounded::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Dash_Grounded::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Dash_Grounded::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
