#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayEffectComponents/AbilitiesGameplayEffectComponent.h"
#include "FTACharacter.generated.h"

class UComboManagerComponent;
class UMotionWarpingComponent;
struct FGameplayEffectSpec;
class UEquipmentManagerComponent;
class UFTACharacterData;
class UWeaponManagerComponent;
class UFTAAbilitySystemComponent;
// class UFTAAttributeSet;
// class UFTAGameplayAbility;
class UGameplayEffect;
class UDidItHitActorComponent;


UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UFTAAbilitySystemComponent> FTAAbilitySystemComponent;
	
public:
	
	UPROPERTY(EditAnywhere, Category = "Character | Abilities")
	TObjectPtr<UFTACharacterData> CharacterAbilitySetData;

	UPROPERTY(BlueprintReadWrite, Category = "Character | Abilities")
	TObjectPtr<UEquipmentManagerComponent> EquipmentManagerComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "MotionWarping")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combo")
	TObjectPtr<UComboManagerComponent> ComboManagerComponent;
	
public:
	
	AFTACharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Character|ASComponent")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponent() const  { return FTAAbilitySystemComponent; };
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	TObjectPtr<UMotionWarpingComponent> GetMotionWarpingComponent();

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual void InitAbilitySystemComponent();

	void AddCharacterBaseAbilities();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TempLaunchCharacter();
	
};