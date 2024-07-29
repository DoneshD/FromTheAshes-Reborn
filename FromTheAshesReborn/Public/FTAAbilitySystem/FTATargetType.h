#pragma once

#include "Abilities/GameplayAbilityTypes.h"
#include "FTAAbilitySystem/FTAAbilityTypes.h"
#include "FTATargetType.generated.h"

class AFTACharacter;
class AActor;
struct FGameplayEventData;


/**
 * Class that is used to determine targeting for abilities
 * It is meant to be blueprinted to run target logic
 * This does not subclass GameplayAbilityTargetActor because this class is never instanced into the world
 * This can be used as a basis for a game-specific targeting blueprint
 * If your targeting is more complicated you may need to instance into the world once or as a pooled actor
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class FROMTHEASHESREBORN_API UFTATargetType : public UObject
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UFTATargetType() {}

	/** Called to determine targets to apply gameplay effects to */
	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(AFTACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
	virtual void GetTargets_Implementation(AFTACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
};

/** Trivial target type that uses the owner */
UCLASS(NotBlueprintable)
class FROMTHEASHESREBORN_API UFTATargetType_UseOwner : public UFTATargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UFTATargetType_UseOwner() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(AFTACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable)
class FROMTHEASHESREBORN_API UFTATargetType_UseEventData : public UFTATargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UFTATargetType_UseEventData() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(AFTACharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};