#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LockOnFunctionLibrary.generated.h"


class UCharacterMovementComponent;

UCLASS()
class FROMTHEASHESREBORN_API ULockOnFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
private:
	// static TObjectPtr<UCharacterMovementComponent> CharacterMovement;
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float AngleFromLockedTarget(AActor* OwningActor, AActor* LockOnTarget);
	
};
