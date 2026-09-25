#include "FTAAbilitySystem/GameplayCues/NiagaraCueNotify.h"

#include "FTAAbilitySystem/GameplayCues/NiagaraCueObject.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

void ANiagaraCueNotify::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
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

			UFTACueObject* CueObject = FTAContext->CueObjects.FindRef(FGameplayTag::RequestGameplayTag("GameplayCue.Niagara.Spawn.Burst"));

			NiagaraCueObject = Cast<UNiagaraCueObject>(CueObject);

			if (!NiagaraCueObject)
			{
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *GetNameSafe(NiagaraCueObject));
			
			break;
		}

	case EGameplayCueEvent::WhileActive:
		{
			break;
		}

	case EGameplayCueEvent::Removed:
		{
			break;
		}

	default:
		break;
	}

	Super::HandleGameplayCue(MyTarget, EventType, Parameters);
	
}
