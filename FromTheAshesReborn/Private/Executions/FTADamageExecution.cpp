#pragma once
#include "Executions/FTADamageExecution.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UFTAAttributeSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

struct FHealthStatics
{
	FGameplayEffectAttributeCaptureDefinition HealthDef;

	FHealthStatics()
	{
		HealthDef = FGameplayEffectAttributeCaptureDefinition(UFTAAttributeSet::GetCurrentHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FHealthStatics& HealthStatics()
{
	static FHealthStatics Statics;
	return Statics;
}

UFTADamageExecution::UFTADamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
	RelevantAttributesToCapture.Add(HealthStatics().HealthDef);
}

void UFTADamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FFTAGameplayEffectContext* TypedContext = FFTAGameplayEffectContext::ExtractEffectContext(Spec.GetContext());
	check(TypedContext);
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float BaseDamage = -10.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealthStatics().HealthDef, EvaluateParameters, BaseDamage);

	
	const AActor* EffectCauser = TypedContext->GetEffectCauser();
	const FHitResult* HitActorResult = TypedContext->GetHitResult();
	
	// Clamping is done when damage is converted to -health
	// const float DamageDone = FMath::Max(BaseDamage, 0.0f);
	
	// if (DamageDone > 0.0f)
	// {
		UE_LOG(LogTemp, Warning, TEXT("OutExecutionOutput"));
		// Apply a damage modifier, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UFTAAttributeSet::GetCurrentHealthAttribute(), EGameplayModOp::Additive, -BaseDamage));
	// }
}
