#include "FTAAbilitySystem/GameplayCues/FTAGameplayCueNotify_BurstLatent.h"

#include "NiagaraSystem.h"
#include "FTAAbilitySystem/GameplayCues/FTACueObject.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

void AFTAGameplayCueNotify_BurstLatent::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
    switch (EventType)
    {
    case EGameplayCueEvent::OnActive:
    {
        if (!Parameters.EffectContext.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("EffectContext is invalid"));
            break;
        }

        FFTAGameplayEffectContext* FTAContext = FFTAGameplayEffectContext::ExtractEffectContext(Parameters.EffectContext);

        if (!FTAContext)
        {
            UE_LOG(LogTemp, Error, TEXT("FTAContext is NULL"));
            break;
        }

        if (!FTAContext->CueObject)
        {
            UE_LOG(LogTemp, Error, TEXT("CueObject is NULL"));
            break;
        }

        FTACueObject = FTAContext->CueObject;

        break;
    }

    case EGameplayCueEvent::WhileActive:
    {
        break;
    }

    case EGameplayCueEvent::Removed:
    {
        FTACueObject = nullptr;
        break;
    }

    default:
        break;
    }

    
    Super::HandleGameplayCue(MyTarget, EventType, Parameters);
}