#include "FTAAbilitySystem/AttributeSets/ArmorAttributeSet.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"

UArmorAttributeSet::UArmorAttributeSet()
	: CurrentArmor(0.0f)
	, MaxArmor(100.0f)
{

	bOutOfArmor = false;
}

bool UArmorAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if(!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	return true;
	
}

void UArmorAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumArmor = 0.0f;
	
	if(Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		if(Data.EvaluatedData.Magnitude > 0.0f)
		{
			SetCurrentArmor(FMath::Clamp(GetCurrentArmor() - GetIncomingDamage(), MinimumArmor, GetMaxArmor()));
			SetIncomingDamage(0.0f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingArmorAttribute())
	{
		SetCurrentArmor(FMath::Clamp(GetCurrentArmor() + GetIncomingArmor(), MinimumArmor, GetMaxArmor()));
		SetIncomingArmor(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetCurrentArmorAttribute())
	{
		SetCurrentArmor(FMath::Clamp(GetCurrentArmor(), MinimumArmor, GetMaxArmor()));
	}
	
	if((GetCurrentArmor() <= 0.0) && !bOutOfArmor)
	{
		if(OnOutOfArmor.IsBound())
		{
			const FGameplayEffectContextHandle EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnOutOfArmor.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
		}
	}

	bOutOfArmor = GetCurrentArmor() <= 0.0f;
}

void UArmorAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UArmorAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);

}

void UArmorAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if(Attribute == GetMaxArmorAttribute())
	{
		if(GetCurrentArmor() > NewValue)
		{
			UFTAAbilitySystemComponent* FTA_ASC = GetFTAAbilitySystemComponent();
			check(FTA_ASC)

			FTA_ASC->ApplyModToAttribute(GetCurrentArmorAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if(bOutOfArmor && (GetCurrentArmor() > 0.0f))
	{
		bOutOfArmor = false;
	}
}

void UArmorAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetCurrentArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxArmor());
	}
	else if (Attribute == GetMaxArmorAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}