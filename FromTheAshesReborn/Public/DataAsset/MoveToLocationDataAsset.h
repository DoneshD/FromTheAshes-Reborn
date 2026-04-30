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

USTRUCT(BlueprintType)
struct FLocationData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Duration = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector EndLocationVector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector RelativeOffsetVector = FVector::ZeroVector;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Params")
	bool GetLockedOnTargetLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Movement | Aerial")
	bool EnableAerialCombat = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Movement | Aerial", meta = (EditCondition = "EnableAerialCombat"))
	TEnumAsByte<EMovementMode> PostMovementMode = MOVE_Falling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FLocationData LocationData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FLocationData TempLocationData;
	
};
