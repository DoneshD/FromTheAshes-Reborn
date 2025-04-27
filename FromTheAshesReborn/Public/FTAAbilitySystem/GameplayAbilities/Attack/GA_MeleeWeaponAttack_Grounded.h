#pragma once

#include "CoreMinimal.h"
#include "GA_MeleeWeaponAttack.h"
#include "GA_MeleeWeaponAttack_Grounded.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack_Grounded : public UGA_MeleeWeaponAttack
{
	GENERATED_BODY()

public:
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

};
