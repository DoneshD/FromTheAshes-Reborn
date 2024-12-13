#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"

UFTAAttributeSet::UFTAAttributeSet()
{
	InitCurrentHealth(100.0f);
	InitMaxHealth(100.0f);
	InitBaseDamage(10.0f);
	InitMoveSpeed(600.0f);

}
UFTAAbilitySystemComponent* UFTAAttributeSet::GetFTAAbilitySystemComponent()
{
	return Cast<UFTAAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

void UFTAAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	// // If a Max value changes, adjust current to keep Current % of Current to Max
	// if (Attribute == GetMaxHealthAttribute()) // GetMaxHealthAttribute comes from the Macros defined at the top of the header
	// {
	// 	AdjustAttributeForMaxChange(CurrentHealth, MaxHealth, NewValue, GetCurrentHealthAttribute());
	// }
	//
	// else if (Attribute == GetMoveSpeedAttribute())
	// {
	// 	// Cannot slow less than 150 units/s and cannot boost more than 1000 units/s
	// 	NewValue = FMath::Clamp<float>(NewValue, 0, 1500);
	// }
}

void UFTAAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// Super::PostGameplayEffectExecute(Data);
	//
	// FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	// UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	// const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	// FGameplayTagContainer SpecAssetTags;
	// Data.EffectSpec.GetAllAssetTags(SpecAssetTags);
	//
	// // Get the Target actor, which should be our owner
	// AActor* TargetActor = nullptr;
	// AController* TargetController = nullptr;
	// AFTACharacter* TargetCharacter = nullptr;
	// if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	// {
	// 	TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
	// 	TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
	// 	TargetCharacter = Cast<AFTACharacter>(TargetActor);
	// }
	//
	// // Get the Source actor
	// AActor* SourceActor = nullptr;
	// AController* SourceController = nullptr;
	// AFTACharacter* SourceCharacter = nullptr;
	// if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	// {
	// 	SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
	// 	SourceController = Source->AbilityActorInfo->PlayerController.Get();
	// 	if (SourceController == nullptr && SourceActor != nullptr)
	// 	{
	// 		if (APawn* Pawn = Cast<APawn>(SourceActor))
	// 		{
	// 			SourceController = Pawn->GetController();
	// 		}
	// 	}
	//
	// 	// Use the controller to find the source pawn
	// 	if (SourceController)
	// 	{
	// 		SourceCharacter = Cast<AFTACharacter>(SourceController->GetPawn());
	// 	}
	// 	else
	// 	{
	// 		SourceCharacter = Cast<AFTACharacter>(SourceActor);
	// 	}
	//
	// 	// Set the causer actor based on context if it's set
	// 	if (Context.GetEffectCauser())
	// 	{
	// 		SourceActor = Context.GetEffectCauser();
	// 	}
	// }
	//
	// if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	// {
	// 	SetCurrentHealth(FMath::Clamp(GetCurrentHealth(), 0.0f, GetMaxHealth()));
	// }
}

void UFTAAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	// UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	// const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	// if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	// {
	// 	// Change current value to maintain the current Val / Max percent
	// 	const float CurrentValue = AffectedAttribute.GetCurrentValue();
	// 	float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;
	//
	// 	AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	// }
}

