#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FTAAbilitySet.generated.h"

struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;
class UFTAAttributeSet;
class UGameplayEffect;
class UFTAAbilitySystemComponent;
class UFTAGameplayAbility;

/*
 *	Data used by the ability set to grant gameplay abilities.
 */

USTRUCT(BlueprintType)
struct FFTAAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Ability"))
	TSubclassOf<UFTAGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AbilityLevel")
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/*
 *	Data used by the ability set to grant gameplay effects.
 */

USTRUCT(BlueprintType)
struct FFTAAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/*
 *	Data used by the ability set to grant attribute sets.
 */

USTRUCT(BlueprintType)
struct FFTAAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFTAAttributeSet> AttributeSet;

};

/*
 *	Data used to store handles to what has been granted by the ability set.
 */

USTRUCT(BlueprintType)
struct FFTAAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UFTAAttributeSet* Set);

	void TakeFromAbilitySystem(UFTAAbilitySystemComponent* FTAASC);

protected:

	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UFTAAttributeSet>> GrantedAttributeSets;
};

UCLASS()
class FROMTHEASHESREBORN_API UFTAAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UFTAAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	void GiveToAbilitySystem(UFTAAbilitySystemComponent* FTA_ASC, FFTAAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:

	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FFTAAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FFTAAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FFTAAbilitySet_AttributeSet> GrantedAttributes;
};
