#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTAGameplayCueNotify_BurstLatent.h"
#include "CameraShakeNotify.generated.h"

class UCameraShakeCueObject;

UCLASS()
class FROMTHEASHESREBORN_API ACameraShakeNotify : public AFTAGameplayCueNotify_BurstLatent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraShakeCueObject* CameraShakeCueObject;

public:

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
	
};
