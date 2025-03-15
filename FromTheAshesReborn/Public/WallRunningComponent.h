#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunningComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UWallRunningComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	bool CheckForWall();
	bool CheckWallRunning();

	void WallRunningMovement();

protected:
	UWallRunningComponent();
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
