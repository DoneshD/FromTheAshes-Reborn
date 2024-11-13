#pragma once

#include "CoreMinimal.h"
#include "GA_FromEquipment.h"
#include "GA_MeleeWeapon.generated.h"

class UFTAMeleeWeaponInstance;
enum ECollisionChannel : int;

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeapon : public UGA_FromEquipment
{
	GENERATED_BODY()

	UGA_MeleeWeapon(const FObjectInitializer& = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="FTA|Ability")
	UFTAMeleeWeaponInstance* GetWeaponIntance() const;

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of UGameplayAbility interface
 };
