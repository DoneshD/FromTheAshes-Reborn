#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FTACustomBase/FTAEnums.h"
#include "FTAGameplayAbility.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UFTAGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	
	UFTAGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability | AbilityID")
	EGAbilityInputID AbilityInputID = EGAbilityInputID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability | AbilityID")
	EGAbilityInputID AbilityID = EGAbilityInputID::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability | Cooldown")
	FScalableFloat CooldownDuration;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability | Cooldown")
	FGameplayTagContainer CooldownTags;

	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;

public:

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual const FGameplayTagContainer* GetCooldownTags() const override;

};
