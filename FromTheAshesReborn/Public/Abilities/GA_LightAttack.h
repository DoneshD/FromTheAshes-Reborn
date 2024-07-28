#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/FTAGameplayAbility.h"
#include "GA_LightAttack.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_LightAttack : public UFTAGameplayAbility
{
	GENERATED_BODY()

protected:


public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData) override;
};
