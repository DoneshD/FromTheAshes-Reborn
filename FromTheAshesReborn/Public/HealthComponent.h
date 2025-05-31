#pragma once

#include "HealthComponent.generated.h"

class UHealthComponent;

class UFTAAbilitySystemComponent;
class UHealthAttributeSet;
class UObject;
struct FFrame;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHealth_AttributeChanged, UHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);



UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UHealthComponent(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintPure)
	static UHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHealthComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable)
	void InitializeWithAbilitySystem(UFTAAbilitySystemComponent* InASC);

	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetHealthNormalized() const;


public:
	UPROPERTY(BlueprintAssignable)
	FHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FHealth_AttributeChanged OnMaxHealthChanged;


protected:

	
	virtual void HandleCurrentHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);


protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UHealthAttributeSet> HealthSet;

};
