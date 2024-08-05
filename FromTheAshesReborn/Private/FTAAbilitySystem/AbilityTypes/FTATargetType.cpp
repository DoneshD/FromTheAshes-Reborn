#include "FTAAbilitySystem/AbilityTypes/FTATargetType.h"
#include "FTACustomBase/FTACharacter.h"

void UFTATargetType::GetTargets_Implementation(AFTACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	return;
}

void UFTATargetType_UseOwner::GetTargets_Implementation(AFTACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingCharacter);
}

void UFTATargetType_UseEventData::GetTargets_Implementation(AFTACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult = EventData.ContextHandle.GetHitResult();
	if (FoundHitResult)
	{
		OutHitResults.Add(*FoundHitResult);
	}
	// else if (EventData.Target)
	// {
	// 	OutActors.Add(const_cast<AActor*>(EventData.Target));
	// }
}