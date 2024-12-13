#include "FTAAbilitySystem/AttributeSets/HealthAttributeSet.h"

#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

UHealthAttributeSet::UHealthAttributeSet()
	: CurrentHealth(100.0f)
	, MaxHealth(100.0f)
{

	bOutOfHealth = false;
}

bool UHealthAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if(!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	//This checks the gameplay effect for any tags that may prevent health from being affected

	//--------------Example--------------//

	/*
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);

			if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
		}
	}
	*/
	
	return true;
	
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	
}

void UHealthAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);

}

void UHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if(Attribute == GetMaxHealthAttribute())
	{
		if(GetCurrentHealth() > NewValue)
		{
			UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent();
			check(FTA_ASC)

			FTA_ASC->ApplyModToAttribute(GetCurrentHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if(bOutOfHealth && (GetCurrentHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UHealthAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetCurrentHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}