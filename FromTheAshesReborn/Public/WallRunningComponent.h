#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunningComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UWallRunningComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float WallRunCheckDistance = 75.0f;
	
	FHitResult WallLeftHitResult;
	FHitResult WallRightHitResult;

	bool bWallLeftHit;
	bool bWallRightHit;

	bool IsWallRunning = false;

public:

	bool CanWallRun(); 
	void CheckForWall();
	void CheckWallRunning();

	void WallRunningMovement();

protected:
	UWallRunningComponent();
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
