#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_BurstLatent.h"
#include "FTAGameplayCueNotify_BurstLatent.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AFTAGameplayCueNotify_BurstLatent : public AGameplayCueNotify_BurstLatent
{
	GENERATED_BODY()

	virtual void HandleGameplayCue(AActor* MyTarget,EGameplayCueEvent::Type EventType,const FGameplayCueParameters& Parameters) override;

	virtual bool OnExecute(AActor* MyTarget, const FGameplayCueParameters& Parameters);

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	
};
