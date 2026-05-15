#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_Slammed.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UGA_Slammed::UGA_Slammed()
{
	ReceiveHitTag = FGameplayTag::RequestGameplayTag("HitTag.Effect.GrantAbility.Slam");
}

void UGA_Slammed::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);
}

bool UGA_Slammed::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Slammed::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation(),
		GetFTACharacterFromActorInfo()->GetActorLocation());
	
	GetFTACharacterFromActorInfo()->SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f));
	
}

void UGA_Slammed::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Slammed::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetFTACharacterFromActorInfo()->RemoveAerialEffects();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
}

void UGA_Slammed::OnSlamComplete()
{
	K2_OnSlamComplete();
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}