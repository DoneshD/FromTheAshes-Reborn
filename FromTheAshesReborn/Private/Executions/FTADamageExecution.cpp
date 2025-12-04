#pragma once

#include "Executions/FTADamageExecution.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "FTAAbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "FTAAbilitySystem/AttributeSets/WeaponAttributeSets/WeaponAttributeSet.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UWeaponAttributeSet::GetBaseWeaponDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UFTADamageExecution::UFTADamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
}

void UFTADamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FFTAGameplayEffectContext* TypedContext = FFTAGameplayEffectContext::ExtractEffectContext(Spec.GetContext());

	if(!TypedContext)
	{
		UE_LOG(LogTemp, Error, TEXT("UFTADamageExecution::Execute_Implementation - TypedContext is Null"))
	}
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);
	
	
	const float DamageDone = FMath::Max(BaseDamage, 0.0f);
	
	if (DamageDone > 0.0f)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), BaseDamage);
		BaseDamage = BaseDamage / 1.25;
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHealthAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, BaseDamage));
	}
}
