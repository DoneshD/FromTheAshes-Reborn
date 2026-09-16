#pragma once

#include "CoreMinimal.h"
#include "FTACueObject.h"
#include "GameplayCueNotify_BurstLatent.h"
#include "FTAGameplayCueNotify_BurstLatent.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AFTAGameplayCueNotify_BurstLatent : public AGameplayCueNotify_BurstLatent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cue")
	TObjectPtr<UFTACueObject> FTACueObject;

public:

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
};