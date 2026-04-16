#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LockOnFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FLockOnAngleResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float Angle;

	UPROPERTY(BlueprintReadOnly)
	FVector InputVector;

	UPROPERTY(BlueprintReadOnly)
	float InputMagnitude;
};

UENUM(BlueprintType)
enum class ELockOnInputOrientationDirection : uint8
{
	None	UMETA(DisplayName = "None"),
	Forward UMETA(DisplayName = "Forward"),
	Backward UMETA(DisplayName = "Backward"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UCLASS()
class FROMTHEASHESREBORN_API ULockOnFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FLockOnAngleResult AngleFromInputVectorToLockedTarget(AActor* OwningActor, AActor* LockOnTarget);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ELockOnInputOrientationDirection GetOrientationOfInput(FLockOnAngleResult AngleResult);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FRotator CheckRotationBasedOnTarget(AActor* OwningActor, AActor* LockOnTarget, FVector Location);
};
