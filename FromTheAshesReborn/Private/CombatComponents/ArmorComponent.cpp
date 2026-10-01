#include "CombatComponents/ArmorComponent.h"

#include "GameplayEffectExtension.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AttributeSets/ArmorAttributeSet.h"

UArmorComponent::UArmorComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	ArmorSet = nullptr;
}

void UArmorComponent::InitializeWithAbilitySystem(UFTAAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ArmorComponent: Armor component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ArmorComponent: Cannot initialize Armor component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	ArmorSet = AbilitySystemComponent->GetSet<UArmorAttributeSet>();
	if (!ArmorSet)
	{
		UE_LOG(LogTemp, Error, TEXT("ArmorComponent: Cannot initialize Armor component for owner [%s] with NULL Armor set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UArmorAttributeSet::GetCurrentArmorAttribute()).AddUObject(this, &ThisClass::HandleCurrentArmorChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UArmorAttributeSet::GetMaxArmorAttribute()).AddUObject(this, &ThisClass::HandleMaxArmorChanged);
	ArmorSet->OnOutOfArmor.AddUObject(this, &ThisClass::HandleOutOfArmor);
	

	AbilitySystemComponent->SetNumericAttributeBase(UArmorAttributeSet::GetCurrentArmorAttribute(), 0.0f);

	OnArmorChanged.Broadcast(this, ArmorSet->GetCurrentArmor(), ArmorSet->GetCurrentArmor(), nullptr);
	OnMaxArmorChanged.Broadcast(this, ArmorSet->GetCurrentArmor(), ArmorSet->GetCurrentArmor(), nullptr);
}

float UArmorComponent::GetArmor() const
{
	return (ArmorSet ? ArmorSet->GetCurrentArmor() : 0.0f);
}

float UArmorComponent::GetMaxArmor() const
{
	return (ArmorSet ? ArmorSet->GetMaxArmor() : 0.0f);
}

float UArmorComponent::GetArmorNormalized() const
{
	if (ArmorSet)
	{
		const float Armor = ArmorSet->GetCurrentArmor();
		const float MaxArmor = ArmorSet->GetMaxArmor();

		return ((MaxArmor > 0.0f) ? (Armor / MaxArmor) : 0.0f);
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

void UArmorComponent::HandleCurrentArmorChanged(const FOnAttributeChangeData& ChangeData)
{
	OnArmorChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UArmorComponent::HandleMaxArmorChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxArmorChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));

}

void UArmorComponent::HandleOutOfArmor(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	OnArmorDepleted.Broadcast();
}
