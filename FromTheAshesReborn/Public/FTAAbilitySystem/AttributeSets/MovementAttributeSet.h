#pragma once

#include "CoreMinimal.h"
#include "FTAAttributeSet.h"
#include "MovementAttributeSet.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UMovementAttributeSet : public UFTAAttributeSet
{
	GENERATED_BODY()

public:
	UMovementAttributeSet();

	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, GroundSpeed);


private:
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData GroundSpeed;
};
