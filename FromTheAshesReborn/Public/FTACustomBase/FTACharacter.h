#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayEffectComponents/AbilitiesGameplayEffectComponent.h"
#include "FTACharacter.generated.h"

class AWeaponActorBase;
class UFTAAbilitySystemComponent;
class UFTACharacterData;
class UEquipmentManagerComponent;
class UMotionWarpingComponent;
class UComboManagerComponent;
class UWallRunningComponent;
class UDidItHitActorComponent;


UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UFTAAbilitySystemComponent> FTAAbilitySystemComponent;
	
public:
	
	UPROPERTY(EditAnywhere, Category = "FTACharacter | Ability Set")
	TObjectPtr<UFTACharacterData> FTACharacterAbilitySetData;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FTACharacter | Core Components")
	TObjectPtr<UEquipmentManagerComponent> EquipmentManagerComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FTACharacter | Core Components")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FTACharacter | Core Components")
	TObjectPtr<UComboManagerComponent> ComboManagerComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "FTACharacter | Core Components")
	TObjectPtr<UWallRunningComponent> WallRunningComponent;

	
public:
	
	AFTACharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Character|ASComponent")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponent() const  { return FTAAbilitySystemComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	TObjectPtr<UMotionWarpingComponent> GetMotionWarpingComponent() const;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual void InitAbilitySystemComponent();

	void AddCharacterBaseAbilities() const;
	
};