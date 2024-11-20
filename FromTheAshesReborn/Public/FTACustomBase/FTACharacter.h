#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "FTAEnums.h"
#include "GameplayTagAssetInterface.h"
#include "Weapons/FTAWeapon.h"
#include "GameplayEffectComponents/AbilitiesGameplayEffectComponent.h"
#include "FTACharacter.generated.h"

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
	
	AFTACharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Character|ASComponent")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponent() const  { return AbilitySystemComponent; };
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual void AddDefaultAbilities();
	virtual void InitializeAttributes();
	virtual void AddStartupEffects();

	

//----------------------------Delete below----------------------//
	
protected:
	FGameplayTag DeadTag;
	
	FGameplayTag EffectRemoveOnDeathTag;

	// Reference to the AttributeSetBase. It will live on the PlayerState or here if the character doesn't have a PlayerState.
	UPROPERTY()
	class UFTAAttributeSet* AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
	FText CharacterName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character | Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Audio")
	class USoundCue* DeathSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability System | Abilities")
	TArray<TSubclassOf<class UFTAGameplayAbility>> DefaultAbilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability System | Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability System | Effects")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Collision")
	TObjectPtr<UDidItHitActorComponent> DidItHitActorComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
	FName WeaponAttachPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TObjectPtr<UWeaponManagerComponent> WeaponManagerComponent;

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FCharacterDiedDelegate OnCharacterDied;


public:

	UPROPERTY(EditAnywhere, Category = "Character | Abilities")
	TObjectPtr<UFTACharacterData> FTACharacterData;

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual int32 GetAbilityLevel(EAbilityInputID AbilityID) const;

	virtual void RemoveCharacterAbilities();

	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void FinishDying();

	/**
	* Getters for attributes from FTAAttributeSetBase
	**/

	UFUNCTION(BlueprintCallable, Category = "Character | Attributes")
	int32 GetCharacterLevel() const;

	virtual void SetCurrentHealth(float Health);

	UFUNCTION(BlueprintCallable, Category = "Character | Attributes")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Character | Attributes")
	float GetMaxHealth() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Character | Attributes")
	float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Character | Attributes")
	float GetMoveSpeedBaseValue() const;
	
	FName GetWeaponAttachPoint();
	
};