#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MoveToLocationDataAsset.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UMoveToLocationDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forward")
	float ForwardDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forward")
	float ForwardLaunchVelocity = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Right")
	float RightDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Right")
	float RightLaunchVelocity = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Up")
	float UpDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Up")
	float UpLaunchVelocity = 0.0f;

	

	
};
