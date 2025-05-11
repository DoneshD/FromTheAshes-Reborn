#pragma once

#include "CoreMinimal.h"
#include "GA_MeleeWeaponAttack.h"
#include "GA_MeleeWeaponAttack_Aerial.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack_Aerial : public UGA_MeleeWeaponAttack
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

};
