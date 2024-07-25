#include "FTAAbilitySystem/FTAGameplayAbility.h"
#include "AbilitySystemComponent.h"

UFTAGameplayAbility::UFTAGameplayAbility()
{
	
}

void UFTAGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if(ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
		
	}
	
}

const FGameplayTagContainer* UFTAGameplayAbility::GetCooldownTags() const
{
	Super::GetCooldownTags();
	
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the ability cooldown tags change (moved to a different slot)
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	
	return MutableTags;
}


