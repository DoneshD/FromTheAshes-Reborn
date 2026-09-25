#include "FTAAbilitySystem/GameplayCues/CameraShakeNotify.h"

#include "FTAAbilitySystem/GameplayCues/CameraShakeCueObject.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

void ACameraShakeNotify::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
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

			UFTACueObject* CueObject = FTAContext->CueObjects.FindRef(FGameplayTag::RequestGameplayTag("GameplayCue.CameraShake.State"));

			CameraShakeCueObject = Cast<UCameraShakeCueObject>(CueObject);

			if (!CameraShakeCueObject)
			{
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *GetNameSafe(CameraShakeCueObject));
			
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
