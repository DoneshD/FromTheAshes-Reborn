#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "FTAPlayerState.generated.h"

class UFTAAbilitySystemComponent;
class UFTAAttributeSet;
class AFTAPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FFTAOnGameplayAttributeValueChangedDelegate, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

UCLASS()
class FROMTHEASHESREBORN_API AFTAPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "PlayerState|ASComponent")
	TObjectPtr<UFTAAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "PlayerState|AttributeSet")
	TObjectPtr<UFTAAttributeSet> AttributeSet;

	UFUNCTION()
	void AddAbilitiesToPlayerASC();
	
public:
	AFTAPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "PlayerState|ASComponent")
	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// AActor Interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "PlayerState|ASComponent")
	UFTAAttributeSet* GetAttributeSet() const;
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	bool IsAlive() const;
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState | Attributes")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState | Attributes")
	float GetMoveSpeed() const;

	FGameplayTag DeadTag;
	FDelegateHandle HealthChangedDelegateHandle;
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

};
