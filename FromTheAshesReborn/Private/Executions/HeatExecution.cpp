#pragma once

#include "Executions/HeatExecution.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "FTAAbilitySystem/AttributeSets/HeatAttributeSet.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

struct FHeatStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHeatDef;

	FHeatStatics()
	{
		BaseHeatDef = FGameplayEffectAttributeCaptureDefinition(UHeatAttributeSet::GetIncomingHeatAttribute(),EGameplayEffectAttributeCaptureSource::Source,true);
	}
};

static FHeatStatics& HeatStatics()
{
	static FHeatStatics Statics;
	return Statics;
}

UHeatExecution::UHeatExecution()
{
	RelevantAttributesToCapture.Add(HeatStatics().BaseHeatDef);
}

void UHeatExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FFTAGameplayEffectContext* TypedContext = FFTAGameplayEffectContext::ExtractEffectContext(Spec.GetContext());

	if(!TypedContext)
	{
		// UE_LOG(LogTemp, Error, TEXT("UHeatExecution::Execute_Implementation - TypedContext is Null"))
	}
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float BaseHeat = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HeatStatics().BaseHeatDef, EvaluateParameters, BaseHeat);
	
	
	const float HeatDone = FMath::Max(BaseHeat, 0.0f);
	
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHeatAttributeSet::GetIncomingHeatAttribute(), EGameplayModOp::Additive, BaseHeat));
	
}
