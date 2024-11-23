#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "FTAEnums.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayEffectComponents/AbilitiesGameplayEffectComponent.h"
#include "FTACharacter.generated.h"

class UEquipmentManagerComponent;
class UFTACharacterData;
class UWeaponManagerComponent;
struct FGameplayEffectSpec;
class UFTAAbilitySystemComponent;
class UFTAAttributeSet;
class UFTAGameplayAbility;
class UGameplayEffect;
class UDidItHitActorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AFTACharacter*, Character);

UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> AbilitySystemComponent;
	
public:
	
	UPROPERTY(EditAnywhere, Category = "Character | Abilities")
	TObjectPtr<UFTACharacterData> FTACharacterData;

	UPROPERTY(BlueprintReadWrite, Category = "Character | Abilities")
	TObjectPtr<UEquipmentManagerComponent> EquipmentManagerComponent;
	
public:
	
	AFTACharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Character|ASComponent")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponent() const  { return AbilitySystemComponent; };
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	
};