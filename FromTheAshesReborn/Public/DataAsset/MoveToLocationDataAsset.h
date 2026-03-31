#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MoveToLocationDataAsset.generated.h"

UENUM(BlueprintType)
enum class ETriggerMovementType : uint8
{
	None,
	OnAbilityActivation,
	OnTagReceived
};

UCLASS()
class FROMTHEASHESREBORN_API UMoveToLocationDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Params")
	ETriggerMovementType TriggerType = ETriggerMovementType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Params")
	bool EndAbilityOnCompleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location Offset")
	float Duration = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location Offset")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocationReached")
	bool SetVelocity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocationReached", meta = (EditCondition = "SetGravity"))
	FVector LocationReachedVelocity = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocationReached")
	bool SetGravity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocationReached", meta = (EditCondition = "SetGravity"))
	float Gravity = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocationReached")
	FVector LaunchVelocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LocationReached")
	bool EnableAerialCombat = false;
	
};
