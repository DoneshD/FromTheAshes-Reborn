#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTAGameplayCueNotify_BurstLatent.h"
#include "OverlayCueNotify.generated.h"


UCLASS()
class FROMTHEASHESREBORN_API AOverlayCueNotify : public AFTAGameplayCueNotify_BurstLatent
{
	GENERATED_BODY()

public:

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

	void TestFunc();

	FTimerHandle TimerHandle;
};
