#pragma once

#include "HeatComponent.generated.h"

class UHeatComponent;

class UFTAAbilitySystemComponent;
class UHeatAttributeSet;
class UObject;
struct FFrame;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHeat_AttributeChanged, UHeatComponent*, HeatComponent, float, OldValue, float, NewValue, AActor*, Instigator);

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UHeatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UHeatComponent(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintPure)
	static UHeatComponent* FindHeatComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHeatComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable)
	void InitializeWithAbilitySystem(UFTAAbilitySystemComponent* InASC);

	UFUNCTION(BlueprintCallable)
	float GetHeat() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxHeat() const;

	UFUNCTION(BlueprintCallable)
	float GetHeatNormalized() const;

public:
	
	UPROPERTY(BlueprintAssignable)
	FHeat_AttributeChanged OnHeatChanged;

	UPROPERTY(BlueprintAssignable)
	FHeat_AttributeChanged OnMaxHeatChanged;

protected:
	
	virtual void HandleCurrentHeatChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHeatChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHeat(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

protected:

	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UHeatAttributeSet> HeatSet;

};
