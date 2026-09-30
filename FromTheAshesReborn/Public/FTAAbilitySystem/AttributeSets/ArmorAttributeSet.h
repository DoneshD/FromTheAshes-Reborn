#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "ArmorAttributeSet.generated.h"

struct FGameplayEffectModCallbackData;



UCLASS()
class FROMTHEASHESREBORN_API UArmorAttributeSet : public UFTAAttributeSet
{
	GENERATED_BODY()

public:

	UArmorAttributeSet();
	
	ATTRIBUTE_ACCESSORS(UArmorAttributeSet, CurrentArmor);
	ATTRIBUTE_ACCESSORS(UArmorAttributeSet, MaxArmor);
	ATTRIBUTE_ACCESSORS(UArmorAttributeSet, IncomingArmor);
	ATTRIBUTE_ACCESSORS(UArmorAttributeSet, IncomingDamage);

	// Delegate to broadcast when the Armor attribute reaches zero.
	mutable FFTAAttributeEvent OnOutOfArmor;

protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	UPROPERTY(BlueprintReadOnly, Category = "Armor | Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentArmor;
	
	UPROPERTY(BlueprintReadOnly, Category = "Armor | Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxArmor;

	// Used to track when the Armor reaches 0.
	bool bOutOfArmor;

	//------------------Meta Attributes------------------//

	UPROPERTY(BlueprintReadOnly, Category = "Armor | Meta Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingArmor;
	
	UPROPERTY(BlueprintReadOnly, Category = "Armor | Meta Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingDamage;
};
