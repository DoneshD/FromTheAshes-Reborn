#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTAGameplayCueNotify_BurstLatent.h"
#include "NiagaraCueNotify.generated.h"

class UNiagaraCueObject;

UCLASS()
class FROMTHEASHESREBORN_API ANiagaraCueNotify : public AFTAGameplayCueNotify_BurstLatent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UNiagaraCueObject* NiagaraCueObject;

public:

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
	
};
