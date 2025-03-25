#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputReadingFunctionLibrary.generated.h"


class UCharacterMovementComponent;

UCLASS()
class FROMTHEASHESREBORN_API UInputReadingFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static FVector CheckInputVector(UCharacterMovementComponent* CMC, float ErrorMargin = 0.0001f);
};
