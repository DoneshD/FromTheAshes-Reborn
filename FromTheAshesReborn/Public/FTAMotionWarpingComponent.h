#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTAMotionWarpingComponent.generated.h"

class UMotionWarpingComponent;
class AFTACharacter;

USTRUCT(BlueprintType)
struct FWarpData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Warp Data")
	FName WarpTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Warp Data")
	AActor* WarpTargetActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Data")
	float WarpTargetLocationOffset;

	FWarpData()
	:
	WarpTargetName("WarpTarget"),
	WarpTargetActor(nullptr),
	WarpTargetLocationOffset(165.0f)
	{}
};


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
	void WarpToTarget(FWarpData WarpData);

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void RemoveWarpTarget();

	
};
