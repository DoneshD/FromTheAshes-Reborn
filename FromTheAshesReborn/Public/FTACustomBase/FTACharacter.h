#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "FTAEnums.h"
#include "GameplayEffectComponents/AbilitiesGameplayEffectComponent.h"
#include "FTACharacter.generated.h"

struct FGameplayEffectSpec;
class UFTAAbilitySystemComponent;
class UFTAAttributeSet;
class UFTAGameplayAbility;
class UGameplayEffect;
// class FActiveGameplayEffectHandle;

UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFTAAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<TSubclassOf<UFTAGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

public:
	AFTACharacter();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	
	int32 GetAbilityLevel(EGAbilityInputID AbilityID);
	
	void GiveDefaultAbilities();

	void InitDefaultAttributes() const;

	void InitGameplayEffectDelegate();

	virtual UFTAAttributeSet* GetAttributeSet() const;

	virtual void OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	//AbilitySystemInterface functions
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//GameplayTagAssetInterface functions
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
};
