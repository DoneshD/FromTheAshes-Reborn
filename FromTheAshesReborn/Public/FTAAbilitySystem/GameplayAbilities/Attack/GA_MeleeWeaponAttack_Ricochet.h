#pragma once

#include "CoreMinimal.h"
#include "GA_MeleeWeaponAttack_Aerial.h"
#include "GA_MeleeWeaponAttack_Ricochet.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGA_MeleeWeaponAttack_Ricochet : public UGA_MeleeWeaponAttack_Aerial
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GroundPound Ability")
	float GroundPoundSpeed = 6000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GroundPound Ability")
	float TraceVerticalDownwardDistance = 3000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GroundPound Ability")
	float SlamDuration = 0.20f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<TSubclassOf<UGA_ReceiveHit>> FollowupReactions;

	FVector TraceStartLocation;
	FVector TraceEndLocation;

public:

	UGA_MeleeWeaponAttack_Ricochet(const FObjectInitializer& = FObjectInitializer::Get());
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FHitDataInfo& HitData) override;

	FVector FindGroundLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	
};
