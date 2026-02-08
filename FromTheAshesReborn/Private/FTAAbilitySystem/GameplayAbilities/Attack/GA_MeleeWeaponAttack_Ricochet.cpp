#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_Ricochet.h"
#include "FTACustomBase/FTACharacter.h"
#include "EventObjects/SlamEventObject.h"

UGA_MeleeWeaponAttack_Ricochet::UGA_MeleeWeaponAttack_Ricochet(const FObjectInitializer&)
{
}

bool UGA_MeleeWeaponAttack_Ricochet::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_MeleeWeaponAttack_Ricochet::SendMeleeHitGameplayEvents(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitDataInfo& HitData)
{

	FVector GroundLocation = FindGroundLocation(TargetDataHandle);
	
	USlamEventObject* SlamInfoObj = NewObject<USlamEventObject>(this);
	SlamInfoObj->SlamData.Speed = DescentSpeed;
	SlamInfoObj->SlamData.DownwardDistance = TraceVerticalDownwardDistance;
	SlamInfoObj->SlamData.Duration = SlamDuration;
	SlamInfoObj->SlamData.Location = GroundLocation;
	SlamInfoObj->HitData.Instigator = GetFTACharacterFromActorInfo();
	// SlamInfoObj->HitData.HitDirection = AttackData.AttackDirectionStruct.AttackDirection;
	SlamInfoObj->HitData.PossibleFollowupReactions = FollowupReactions;
	
	OnHitEventData.OptionalObject = SlamInfoObj;
	
	UGA_MeleeAttack::SendMeleeHitGameplayEvents(TargetDataHandle, HitData);
}

FVector UGA_MeleeWeaponAttack_Ricochet::FindGroundLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	FHitResult TempHitResult;
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
	TraceEndLocation = TargetActor->GetActorLocation() + FVector(0.0f, 0.0f, -TraceVerticalDownwardDistance);

	FCollisionObjectQueryParams GroundCollisionQueryParams;
	GroundCollisionQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	GroundCollisionQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

					
	if(bool bHitResult = GetWorld()->LineTraceSingleByObjectType(TempHitResult, TargetActor->GetActorLocation(), TraceEndLocation, GroundCollisionQueryParams))
	{
		return TempHitResult.ImpactPoint;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Ground not found"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return FVector(0, 0, 0);
	}
}
