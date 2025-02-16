#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ParkourFunctionLibrary.generated.h"

struct FGameplayTag;

UCLASS()
class FROMTHEASHESREBORN_API UParkourFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FRotator NormalReverseRotationZ(FVector NormalVector);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FRotator ReverseRotation(FRotator Rotation);

};
