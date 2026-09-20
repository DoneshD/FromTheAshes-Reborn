#include "FTAAbilitySystem/GameplayCues/OverlayCueNotify.h"

#include "FTAAbilitySystem/GameplayCues/MaterialCueObject.h"

void AOverlayCueNotify::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters){
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);

	switch (EventType)
	{
	case EGameplayCueEvent::OnActive:
		{
			if(FTACueObject)
			{
				UMaterialCueObject* MCO = Cast<UMaterialCueObject>(FTACueObject);
				if(MCO)
				{
					UE_LOG(LogTemp, Warning, TEXT("MCO: %s"), *GetNameSafe(MCO));
				}

				GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,            
				this,                      
				&AOverlayCueNotify::TestFunc, 
				.001f,                     
				true                     
			);

			}
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
	
}

void AOverlayCueNotify::TestFunc()
{
	UE_LOG(LogTemp, Warning, TEXT("HERERERE"));
}
