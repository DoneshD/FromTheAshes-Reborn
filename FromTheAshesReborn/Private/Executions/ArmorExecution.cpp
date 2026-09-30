#pragma once

#include "Executions/ArmorExecution.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "FTAAbilitySystem/AttributeSets/ArmorAttributeSet.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

struct FArmorStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseArmorDef;

	FArmorStatics()
	{
		BaseArmorDef = FGameplayEffectAttributeCaptureDefinition(UArmorAttributeSet::GetIncomingArmorAttribute(),EGameplayEffectAttributeCaptureSource::Source,true);
	}
};

static FArmorStatics& ArmorStatics()
{
	static FArmorStatics Statics;
	return Statics;
}

UArmorExecution::UArmorExecution()
{
	RelevantAttributesToCapture.Add(ArmorStatics().BaseArmorDef);
}

void UArmorExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FFTAGameplayEffectContext* TypedContext = FFTAGameplayEffectContext::ExtractEffectContext(Spec.GetContext());

	if(!TypedContext)
	{
		// UE_LOG(LogTemp, Error, TEXT("UArmorExecution::Execute_Implementation - TypedContext is Null"))
	}
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float BaseArmor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ArmorStatics().BaseArmorDef, EvaluateParameters, BaseArmor);
	
	const float ArmorDone = FMath::Max(BaseArmor, 0.0f);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UArmorAttributeSet::GetIncomingArmorAttribute(), EGameplayModOp::Additive, BaseArmor));
	
}
