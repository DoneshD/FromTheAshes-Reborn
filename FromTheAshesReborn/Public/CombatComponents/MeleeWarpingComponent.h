#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack.h"
#include "MeleeWarpingComponent.generated.h"

class UMotionWarpingComponent;
class AFTAPlayerState;
class AFTACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UMeleeWarpingComponent : public UActorComponent
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
	FMeleeMeleeWarpData CurrentWarpData;

public:
	
	UMeleeWarpingComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateWarpTarget(FVector TargetLocation, FRotator TargetRotation);
	float GetDistanceFromActorToAxis(AActor* ActorToCheck, FVector TraceStartLocation, FVector TraceAxis);

	AActor* FilterClosestActorToAxisTrace(TArray<FHitResult> HitArray, FVector TraceStartLocation, FVector TraceEndLocation);
	AActor* FilterClosestActorToOwner(TArray<FHitResult> HitArray);
	
	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void WarpToTarget(FMeleeMeleeWarpData WarpData);

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void RemoveWarpTarget();
	
	FVector GetTraceDirection();
	
};
