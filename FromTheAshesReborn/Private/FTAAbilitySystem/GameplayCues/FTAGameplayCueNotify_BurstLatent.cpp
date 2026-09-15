#include "FTAAbilitySystem/GameplayCues/FTAGameplayCueNotify_BurstLatent.h"

#include "NiagaraSystem.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

void AFTAGameplayCueNotify_BurstLatent::HandleGameplayCue(
	AActor* MyTarget,
	EGameplayCueEvent::Type EventType,
	const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);

	const FFTAGameplayEffectContext* FTAContext =
		FFTAGameplayEffectContext::ExtractEffectContext(
			Parameters.EffectContext
		);

	if (FTAContext && FTAContext->NiagaraSystem)
	{
		UNiagaraSystem* NiagaraSystem = FTAContext->NiagaraSystem;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Niagara System: %s"),
			*NiagaraSystem->GetName()
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid niagara"))
	}
}


bool AFTAGameplayCueNotify_BurstLatent::OnExecute(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	
	UE_LOG(LogTemp, Warning, TEXT("FTA Gameplay Cue Fired - OnExecute"));

	return true;
}

bool AFTAGameplayCueNotify_BurstLatent::OnExecute_Implementation(AActor* MyTarget,
	const FGameplayCueParameters& Parameters)
{
	UE_LOG(LogTemp, Warning, TEXT("FTA Gameplay Cue Fired - OnExecuteImplementation"));
	
	return Super::OnExecute_Implementation(MyTarget, Parameters);
}
