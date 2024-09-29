#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "FTAEnums.h"
#include "GameplayEffectComponents/AbilitiesGameplayEffectComponent.h"
#include "FTACharacter.generated.h"

struct FGameplayEffectSpec;
class UFTAAbilitySystemComponent;
class UFTAAttributeSet;
class UFTAGameplayAbility;
class UGameplayEffect;
class UDidItHitActorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AFTACharacter*, Character);

UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFTACharacter(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FCharacterDiedDelegate OnCharacterDied;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

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
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
	FName WeaponAttachPoint;
	
	FName GetWeaponAttachPoint();

protected:
	FGameplayTag DeadTag;
	
	FGameplayTag EffectRemoveOnDeathTag;
	
	// Reference to the ASC. It will live on the PlayerState or here if the character doesn't have a PlayerState.
	UPROPERTY()
	class UFTAAbilitySystemComponent* AbilitySystemComponent;

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

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Collision")
	TObjectPtr<UDidItHitActorComponent> DidItHitActorComponent;

	virtual void BeginPlay() override;

	virtual void AddDefaultAbilities();

	virtual void InitializeAttributes();

	virtual void AddStartupEffects();


	virtual void SetCurrentHealth(float Health);
};
