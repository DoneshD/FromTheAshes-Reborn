#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTAGameplayCueNotify_BurstLatent.h"
#include "SoundCueNotify.generated.h"

class USoundCueObject;

UCLASS()
class FROMTHEASHESREBORN_API ASoundCueNotify : public AFTAGameplayCueNotify_BurstLatent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USoundCueObject* SoundCueObject;

public:

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
	
};
