#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_GroundPound.h"

#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/AT_SlamCharacterAndWait.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"

UGA_MeleeWeaponAttack_GroundPound::UGA_MeleeWeaponAttack_GroundPound(const FObjectInitializer&): TraceStartLocation(),
                                                                                                 TraceEndLocation(),
                                                                                                 GroundPoundEndLocation()
{
}

void UGA_MeleeWeaponAttack_GroundPound::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
	
}

bool UGA_MeleeWeaponAttack_GroundPound::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
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

void UGA_MeleeWeaponAttack_GroundPound::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//TODO: Need to change this
	GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent")));
	
	FHitResult HitResult;
	TraceStartLocation = ActorInfo->AvatarActor.Get()->GetActorLocation();
	TraceEndLocation = TraceStartLocation + FVector(0.0f, 0.0f, -TraceVerticalDownwardDistance);

	FCollisionObjectQueryParams CollisionQueryParams;
	CollisionQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	CollisionQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	

	if(bool bHitResult = GetWorld()->LineTraceSingleByObjectType(HitResult, ActorInfo->AvatarActor.Get()->GetActorLocation(), TraceEndLocation, CollisionQueryParams))
	{
		GroundPoundEndLocation = HitResult.ImpactPoint;
		DrawDebugSphere(GetWorld(), GroundPoundEndLocation, 12, 12, FColor::Green, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Ground not found"));
	}
	
	DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Red, false, 2.0f, 0, 2.0f);


	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, 0.0f);
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 0.0;

	GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("CombatMovementTag.Slam"));
	
	SlamTask = UAT_SlamCharacterAndWait::AT_SlamCharacterAndWait(
		this,
		GroundPoundEndLocation,
		GroundPoundSpeed,
		SlamDuration);
}

void UGA_MeleeWeaponAttack_GroundPound::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeWeaponAttack_GroundPound::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeWeaponAttack_GroundPound::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeWeaponAttack_GroundPound::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_MeleeWeaponAttack_GroundPound::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if(!UTagValidationFunctionLibrary::IsRegisteredGameplayTag(EventTag))
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack_Launcher::EventMontageReceived - EventTag is invalid"))
		return;
	}
	
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("CombatMovementTag.TestTag2")))
	{
		if (SlamTask)
		{
			SlamTask->OnSlamComplete.AddDynamic(this, &UGA_MeleeWeaponAttack_GroundPound::OnSlamComplete);
			SlamTask->ReadyForActivation();
		}
	}
}

void UGA_MeleeWeaponAttack_GroundPound::SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	Super::SendMeleeHitGameplayEvents(TargetDataHandle);
	
}

void UGA_MeleeWeaponAttack_GroundPound::OnSlamComplete()
{
	GetFTAAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("CombatMovementTag.Slam"));
	PlayAbilityAnimMontage(SlamMontage);
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 4.0;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
