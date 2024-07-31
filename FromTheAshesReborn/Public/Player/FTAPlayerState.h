﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "FTAPlayerState.generated.h"

class UFTAAbilitySystemComponent;
class UFTAAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFTAOnGameplayAttributeValueChangedDelegate, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

UCLASS()
class FROMTHEASHESREBORN_API AFTAPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AFTAPlayerState();

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UFTAAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	bool IsAlive() const;
	
	/**
	* Getters for attributes from GDAttributeSetBase. Returns Current Value unless otherwise specified.
	*/

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Attributes")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Attributes")
	float GetMoveSpeed() const;

protected:
	FGameplayTag DeadTag;

	UPROPERTY()
	class UFTAAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UFTAAttributeSet* AttributeSet;

	// Attribute changed delegate handles
	FDelegateHandle HealthChangedDelegateHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

};
