#include "FTAAbilitySystem/GameplayCues/DecalCueNotify.h"

#include "FTAAbilitySystem/GameplayCues/DecalCueObject.h"
#include "FTAAbilitySystem/GameplayEffects/FTAGameplayEffectContext.h"

void ADecalCueNotify::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
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

			UFTACueObject* CueObject = FTAContext->CueObjects.FindRef(FGameplayTag::RequestGameplayTag("GameplayCue.Decal"));

			DecalCueObject = Cast<UDecalCueObject>(CueObject);

			if (!DecalCueObject)
			{
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *GetNameSafe(DecalCueObject));
			
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
