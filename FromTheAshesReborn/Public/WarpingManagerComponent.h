#pragma once

#include "CoreMinimal.h"
#include "MotionWarpingComponent.h"
#include "Components/ActorComponent.h"
#include "WarpingManagerComponent.generated.h"


class AFTAPlayerState;
class AFTACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UWarpingManagerComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTACharacter = nullptr;

	UPROPERTY()
	AFTAPlayerState* PlayerState = nullptr;

	UPROPERTY()
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent = nullptr;

public:

	FName CurrentWarpTargetName;

	float MotionWarpLocationOffset = 0.0f;

public:
	
	UWarpingManagerComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddWarpTarget(FVector TargetLocation, FRotator TargetRotation);
	
	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void TraceForTarget(FName WarpTargetName,
		float StartTraceLocationOffset = 100,
		float EndTraceLocationDistance = 800,
		float TraceRadius = 100.0f,
		float WarpTargetLocationOffset = 165.0f);

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void RemoveWarpTarget();
	
	FVector GetTraceDirection();
	
};
