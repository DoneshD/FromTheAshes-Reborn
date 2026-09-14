#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "FTACueObject.generated.h"

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class FROMTHEASHESREBORN_API UFTACueObject : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "GameplayCue"))
	FGameplayTag CueTag;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool ActivateAllCues = false;
	
};
