#pragma once

#include "ArmorComponent.generated.h"

class UArmorComponent;

class UFTAAbilitySystemComponent;
class UArmorAttributeSet;
class UObject;
struct FFrame;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FArmor_AttributeChanged, UArmorComponent*, ArmorComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArmorDepleted);


UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UArmorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UArmorComponent(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintPure)
	static UArmorComponent* FindArmorComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UArmorComponent>() : nullptr); }

	UFUNCTION(BlueprintCallable)
	void InitializeWithAbilitySystem(UFTAAbilitySystemComponent* InASC);

	UFUNCTION(BlueprintCallable)
	float GetArmor() const;

	UFUNCTION(BlueprintCallable)
	float GetMaxArmor() const;

	UFUNCTION(BlueprintCallable)
	float GetArmorNormalized() const;

public:
	
	UPROPERTY(BlueprintAssignable)
	FArmor_AttributeChanged OnArmorChanged;

	UPROPERTY(BlueprintAssignable)
	FArmor_AttributeChanged OnMaxArmorChanged;

	UPROPERTY(BlueprintAssignable)
	FOnArmorDepleted OnArmorDepleted;

public:
	
	virtual void HandleCurrentArmorChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxArmorChanged(const FOnAttributeChangeData& ChangeData);

	UFUNCTION(BlueprintCallable)
	virtual void HandleOutOfArmor(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

protected:

	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UArmorAttributeSet> ArmorSet;

};
