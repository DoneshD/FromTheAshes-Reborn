#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "HealthAttributeSet.generated.h"

struct FGameplayEffectModCallbackData;



UCLASS()
class FROMTHEASHESREBORN_API UHealthAttributeSet : public UFTAAttributeSet
{
	GENERATED_BODY()

public:

	UHealthAttributeSet();
	
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, CurrentHealth);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, IncomingHeal);
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, IncomingDamage);

	// Delegate to broadcast when the health attribute reaches zero.
	mutable FFTAAttributeEvent OnOutOfHealth;

protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	UPROPERTY(BlueprintReadOnly, Category = "Health | Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentHealth;
	
	UPROPERTY(BlueprintReadOnly, Category = "Health | Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the health reaches 0.
	bool bOutOfHealth;

	//------------------Meta Attributes------------------//

	UPROPERTY(BlueprintReadOnly, Category = "Health | Meta Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingHeal;
	
	UPROPERTY(BlueprintReadOnly, Category = "Health | Meta Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingDamage;
};
