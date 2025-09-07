#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack.h"
#include "MeleeWarpingComponent.generated.h"

class UMotionWarpingComponent;
class AFTAPlayerState;
class AFTACharacter;

USTRUCT(BlueprintType)
struct FMeleeWarpData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Warp Data")
	FName WarpTargetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Data")
	float StartTraceLocationOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Data")
	float EndTraceLocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Data")
	float TraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warp Data")
	float WarpTargetLocationOffset;

	FMeleeWarpData()
	:
	WarpTargetName("MeleeAttackTarget"),
	StartTraceLocationOffset(100.0f),
	EndTraceLocationOffset(800.0f),
	TraceRadius(600.0f),
	WarpTargetLocationOffset(165.0f)
	{}
};

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
	FMeleeWarpData CurrentWarpData;

public:
	
	UMeleeWarpingComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateWarpTarget(FVector TargetLocation, FRotator TargetRotation);
	float GetDistanceFromActorToAxis(AActor* ActorToCheck, FVector TraceStartLocation, FVector TraceAxis);

	AActor* FilterClosestActorToAxisTrace(TArray<FHitResult> HitArray, FVector TraceStartLocation, FVector TraceEndLocation);
	AActor* FilterClosestActorToOwner(TArray<FHitResult> HitArray);
	
	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void WarpToTarget(FMeleeWarpData WarpData);

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void RemoveWarpTarget();
	
	FVector GetTraceDirection();
	
};
