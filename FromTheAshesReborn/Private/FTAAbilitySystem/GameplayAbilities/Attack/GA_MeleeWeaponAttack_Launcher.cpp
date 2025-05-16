#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_Launcher.h"

#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "Player/FTAPlayerController.h"
#include "Player/FTAPlayerState.h"

UGA_MeleeWeaponAttack_Launcher::UGA_MeleeWeaponAttack_Launcher()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_MeleeWeaponAttack_Launcher::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if(IsLaunching)
	{
		UpdateLauncherMovement(DeltaTime);
	}
}
bool UGA_MeleeWeaponAttack_Launcher::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	//Return true if owner is not player
	AFTAPlayerState* PS = Cast<AFTAPlayerState>(ActorInfo->OwnerActor.Get());
	if(ActorInfo->OwnerActor.Get() != PS)
	{
		return true;
	}
	
	if(!PS->HardLockedTargetActor)
	{
		return false;
	}
	
	float AngleOfInput = ULockOnFunctionLibrary::AngleFromInputVectorToLockedTarget(ActorInfo->AvatarActor.Get(), PS->HardLockedTargetActor);
	
	ELockOnInputOrientationDirection InputOrientation = ULockOnFunctionLibrary::GetOrientationOfInput(AngleOfInput);
	if(InputOrientation != ELockOnInputOrientationDirection::Backward)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_MeleeWeaponAttack_Launcher::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	LauncherElapsedTime = 0.0f;
	LauncherStartTime = GetWorld()->GetTimeSeconds();

	LauncherStartLocation = ActorInfo->AvatarActor->GetActorLocation();
	LauncherEndLocation = ActorInfo->AvatarActor->GetActorLocation() + FVector(0.0f, 0.0f, LauncherVerticalDistance);

}

void UGA_MeleeWeaponAttack_Launcher::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeWeaponAttack_Launcher::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeWeaponAttack_Launcher::UpdateLauncherMovement(float DeltaTime)
{
	LauncherElapsedTime += DeltaTime;

	const float Alpha = FMath::Clamp(LauncherElapsedTime / LauncherDuration, 0.0f, 1.0f);
	const FVector NewLocation = FMath::Lerp(LauncherStartLocation, LauncherEndLocation, Alpha);

	FHitResult Hit;
	CurrentActorInfo->AvatarActor->SetActorLocation(NewLocation, true, &Hit);

	if (Alpha >= 1.0f || Hit.bBlockingHit)
	{
		LauncherLocationReached();
	}
}

void UGA_MeleeWeaponAttack_Launcher::LauncherLocationReached()
{
	IsLaunching = false;
	
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity.Z = 0.0f;
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(AerialStallTimerHandle, this, &UGA_MeleeWeaponAttack_Launcher::EndAirStall, StallDuration, false);

}

void UGA_MeleeWeaponAttack_Launcher::EndAirStall()
{
	
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity.Z = -100.0f;
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 4.0f;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

}

void UGA_MeleeWeaponAttack_Launcher::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeWeaponAttack_Launcher::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_MeleeWeaponAttack_Launcher::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::EventMontageReceived(EventTag, EventData);

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("LauncherTag.Montage.Launch")))
	{
		IsLaunching = true;
	}
	
}