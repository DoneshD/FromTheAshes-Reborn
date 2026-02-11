#pragma once

#include "CoreMinimal.h"
#include "CombatComponents/MeleeWarpingComponent.h"
#include "Components/ActorComponent.h"
#include "FTAMotionWarpingComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UFTAMotionWarpingComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTACharacter = nullptr;

	UPROPERTY()
	TObjectPtr<AFTAPlayerState> PlayerState = nullptr;

	UPROPERTY()
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent = nullptr;

public:

	FName CurrentWarpTargetName;
	FMeleeWarpData CurrentWarpData;

protected:
	UFTAMotionWarpingComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateWarpTarget(FVector TargetLocation, FRotator TargetRotation);
	float GetDistanceFromActorToAxis(AActor* ActorToCheck, FVector TraceStartLocation, FVector TraceAxis);

	AActor* FilterClosestActorToAxisTrace(TArray<FHitResult> HitArray, FVector TraceStartLocation, FVector TraceEndLocation);
	AActor* FilterClosestActorToOwner(TArray<FHitResult> HitArray);
	
	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void WarpToTarget(FMeleeWarpData WarpData, bool InvertLocation = false, bool TestHitWarp = false);

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void RemoveWarpTarget();
	
	FVector GetTraceDirection();
	
};
