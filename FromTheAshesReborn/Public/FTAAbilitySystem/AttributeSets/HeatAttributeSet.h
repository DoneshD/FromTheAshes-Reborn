#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "HeatAttributeSet.generated.h"

struct FGameplayEffectModCallbackData;



UCLASS()
class FROMTHEASHESREBORN_API UHeatAttributeSet : public UFTAAttributeSet
{
	GENERATED_BODY()

public:

	UHeatAttributeSet();
	
	ATTRIBUTE_ACCESSORS(UHeatAttributeSet, CurrentHeat);
	ATTRIBUTE_ACCESSORS(UHeatAttributeSet, MaxHeat);
	ATTRIBUTE_ACCESSORS(UHeatAttributeSet, IncomingHeat);
	ATTRIBUTE_ACCESSORS(UHeatAttributeSet, IncomingDamage);

	// Delegate to broadcast when the Heat attribute reaches zero.
	mutable FFTAAttributeEvent OnOutOfHeat;

protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	UPROPERTY(BlueprintReadOnly, Category = "Heat | Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentHeat;
	
	UPROPERTY(BlueprintReadOnly, Category = "Heat | Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHeat;

	// Used to track when the Heat reaches 0.
	bool bOutOfHeat;

	//------------------Meta Attributes------------------//

	UPROPERTY(BlueprintReadOnly, Category = "Heat | Meta Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingHeat;
	
	UPROPERTY(BlueprintReadOnly, Category = "Heat | Meta Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingDamage;
};
