#include "FTAAbilitySystem/FTAAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UFTAAttributeSet::UFTAAttributeSet()
{
	InitCurrentHealth(100.0f);
	InitMaxHealth(100.0f);
	InitBaseDamage(100.0f);


}

void UFTAAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UAttributeSet::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UFTAAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFTAAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

}

void UFTAAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if(Attribute == GetCurrentHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	
}

void UFTAAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if(Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		SetCurrentHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}

	
	/*
	Example of getting the owning actor of the instigators ASC
	
	UAbilitySystemComponent* ASC = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();
	AActor* ASCOwner = ASC->AbilityActorInfo->OwnerActor.Get();
	ASCOwner->GetActorLocation();
	*/
}

void UFTAAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFTAAttributeSet, Health, OldHealth);
}

void UFTAAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFTAAttributeSet, MaxHealth, OldMaxHealth);
}


