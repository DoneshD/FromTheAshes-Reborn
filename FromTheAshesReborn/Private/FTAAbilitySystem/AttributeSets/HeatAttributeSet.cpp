#include "FTAAbilitySystem/AttributeSets/HeatAttributeSet.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"

UHeatAttributeSet::UHeatAttributeSet()
	: CurrentHeat(100.0f)
	, MaxHeat(100.0f)
{

	bOutOfHeat = false;
}

bool UHeatAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if(!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	return true;
	
}

void UHeatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHeat = 0.0f;
	
	if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		if(Data.EvaluatedData.Magnitude > 0.0f)
		{
			SetCurrentHeat(FMath::Clamp(GetCurrentHeat() - GetIncomingDamage(), MinimumHeat, GetMaxHeat()));
			SetIncomingDamage(0.0f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingHeatAttribute())
	{
		SetCurrentHeat(FMath::Clamp(GetCurrentHeat() + GetIncomingHeat(), MinimumHeat, GetMaxHeat()));
		SetIncomingHeat(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetCurrentHeatAttribute())
	{
		SetCurrentHeat(FMath::Clamp(GetCurrentHeat(), MinimumHeat, GetMaxHeat()));
	}
	
	if((GetCurrentHeat() <= 0.0) && !bOutOfHeat)
	{
		if(OnOutOfHeat.IsBound())
		{
			const FGameplayEffectContextHandle EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnOutOfHeat.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
		}
	}

	bOutOfHeat = GetCurrentHeat() <= 0.0f;
}

void UHeatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UHeatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);

}

void UHeatAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if(Attribute == GetMaxHeatAttribute())
	{
		if(GetCurrentHeat() > NewValue)
		{
			UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent();
			check(FTA_ASC)

			FTA_ASC->ApplyModToAttribute(GetCurrentHeatAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if(bOutOfHeat && (GetCurrentHeat() > 0.0f))
	{
		bOutOfHeat = false;
	}
}

void UHeatAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetCurrentHeatAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHeat());
	}
	else if (Attribute == GetMaxHeatAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}