#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTAMotionWarpingComponent.generated.h"

class UMotionWarpingComponent;
class AFTACharacter;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UFTAMotionWarpingComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTACharacter = nullptr;

	UPROPERTY()
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent = nullptr;

public:

	FName CurrentWarpTargetName;

protected:
	
	UFTAMotionWarpingComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateWarpTarget(FVector TargetLocation, FRotator TargetRotation);

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void WarpToTargetActor(FWarpData WarpData);
	
	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void WarpToTargetLocation(FWarpData WarpData);

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void RemoveWarpTarget();

	
};
