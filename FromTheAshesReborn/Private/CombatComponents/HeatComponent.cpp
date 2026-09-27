#include "CombatComponents/HeatComponent.h"

#include "GameplayEffectExtension.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AttributeSets/HeatAttributeSet.h"

UHeatComponent::UHeatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HeatSet = nullptr;
}

void UHeatComponent::InitializeWithAbilitySystem(UFTAAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HeatComponent: Heat component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HeatComponent: Cannot initialize Heat component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	HeatSet = AbilitySystemComponent->GetSet<UHeatAttributeSet>();
	if (!HeatSet)
	{
		UE_LOG(LogTemp, Error, TEXT("HeatComponent: Cannot initialize Heat component for owner [%s] with NULL Heat set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHeatAttributeSet::GetCurrentHeatAttribute()).AddUObject(this, &ThisClass::HandleCurrentHeatChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHeatAttributeSet::GetMaxHeatAttribute()).AddUObject(this, &ThisClass::HandleMaxHeatChanged);
	HeatSet->OnOutOfHeat.AddUObject(this, &ThisClass::HandleOutOfHeat);

	AbilitySystemComponent->SetNumericAttributeBase(UHeatAttributeSet::GetCurrentHeatAttribute(), HeatSet->GetMaxHeat());

	OnHeatChanged.Broadcast(this, HeatSet->GetCurrentHeat(), HeatSet->GetCurrentHeat(), nullptr);
	OnMaxHeatChanged.Broadcast(this, HeatSet->GetCurrentHeat(), HeatSet->GetCurrentHeat(), nullptr);
}

float UHeatComponent::GetHeat() const
{
	return (HeatSet ? HeatSet->GetCurrentHeat() : 0.0f);
}

float UHeatComponent::GetMaxHeat() const
{
	return (HeatSet ? HeatSet->GetMaxHeat() : 0.0f);
}

float UHeatComponent::GetHeatNormalized() const
{
	if (HeatSet)
	{
		const float Heat = HeatSet->GetCurrentHeat();
		const float MaxHeat = HeatSet->GetMaxHeat();

		return ((MaxHeat > 0.0f) ? (Heat / MaxHeat) : 0.0f);
	}

	return 0.0f;
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

void UHeatComponent::HandleCurrentHeatChanged(const FOnAttributeChangeData& ChangeData)
{
	OnHeatChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UHeatComponent::HandleMaxHeatChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxHeatChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));

}

void UHeatComponent::HandleOutOfHeat(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	//TODO: Later use
}
