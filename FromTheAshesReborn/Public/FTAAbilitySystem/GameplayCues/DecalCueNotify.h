#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTAGameplayCueNotify_BurstLatent.h"
#include "DecalCueNotify.generated.h"

class UDecalCueObject;

UCLASS()
class FROMTHEASHESREBORN_API ADecalCueNotify : public AFTAGameplayCueNotify_BurstLatent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UDecalCueObject* DecalCueObject;

public:

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
	
};
