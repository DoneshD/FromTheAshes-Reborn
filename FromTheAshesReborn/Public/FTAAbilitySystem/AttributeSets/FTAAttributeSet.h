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


class UFTAAbilitySystemComponent;
DECLARE_MULTICAST_DELEGATE_FourParams(FFTAAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
                                      const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);


UCLASS()
class FROMTHEASHESREBORN_API UFTAAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UFTAAttributeSet();

	UFTAAbilitySystemComponent* GetFTAAbilitySystemComponent();
	
};
