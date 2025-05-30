﻿#include "FTAAbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"

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

	return true;
	
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;
	
	if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		if(Data.EvaluatedData.Magnitude > 0.0f)
		{
			SetCurrentHealth(FMath::Clamp(GetCurrentHealth() - GetIncomingDamage(), MinimumHealth, GetMaxHealth()));
			SetIncomingDamage(0.0f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingHealAttribute())
	{
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth() + GetIncomingHeal(), MinimumHealth, GetMaxHealth()));
		SetIncomingHeal(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), MinimumHealth, GetMaxHealth()));
	}
	
	if((GetCurrentHealth() <= 0.0) && !bOutOfHealth)
	{
		if(OnOutOfHealth.IsBound())
		{
			const FGameplayEffectContextHandle EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
		}
	}

	bOutOfHealth = GetCurrentHealth() <= 0.0f;
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