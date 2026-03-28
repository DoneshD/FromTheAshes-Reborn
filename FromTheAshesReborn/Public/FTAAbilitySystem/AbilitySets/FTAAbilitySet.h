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

USTRUCT(BlueprintType)
struct FFTAAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (Categories = "Ability"))
	TSubclassOf<UFTAGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AbilityLevel")
	int32 AbilityLevel = 1;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (Categories = "ActivationGroupTag"))
	FGameplayTag DefaultActivationGroupTag;
};

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

USTRUCT(BlueprintType)
struct FFTAAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFTAAttributeSet> AttributeSet;

};

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

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<UFTAAttributeSet>> GrantedAttributeSets;
};

UCLASS()
class FROMTHEASHESREBORN_API UFTAAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UFTAAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void GiveToAbilitySystem(UFTAAbilitySystemComponent* FTAASC, FFTAAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

	UFUNCTION(BlueprintCallable)
	void AddAbilityToAbilitySet(UFTAAbilitySystemComponent* FTAASC, FFTAAbilitySet_GameplayAbility GameplayAbilityToAdd);


protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FFTAAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FFTAAbilitySet_GameplayEffect> GrantedGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FFTAAbilitySet_AttributeSet> GrantedAttributes;
};
