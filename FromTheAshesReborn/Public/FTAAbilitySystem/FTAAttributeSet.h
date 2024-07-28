#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "FTAAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 

UCLASS()
class FROMTHEASHESREBORN_API UFTAAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Ability | Gameplay Attributes")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UFTAAttributeSet, CurrentHealth);

	UPROPERTY(BlueprintReadOnly, Category = "Ability | Gameplay Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UFTAAttributeSet, MaxHealth);
	
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UFTAAttributeSet, BaseDamage);
	
public:
	
	UFTAAttributeSet();

	virtual  void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual  void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	

	
};
