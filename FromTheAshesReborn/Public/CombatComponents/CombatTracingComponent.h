#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatTracingComponent.generated.h"

class UMotionWarpingComponent;
class AFTAPlayerState;
class AFTACharacter;

USTRUCT(BlueprintType)
struct FCombatTraceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Data")
	float StartTraceLocationOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Data")
	float EndTraceLocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace Data")
	float TraceRadius;

	FCombatTraceData()
	:
	StartTraceLocationOffset(100.0f),
	EndTraceLocationOffset(800.0f),
	TraceRadius(600.0f)
	{}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCombatTracingComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTACharacter = nullptr;

	UPROPERTY()
	TObjectPtr<AFTAPlayerState> PlayerState = nullptr;

protected:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> EnemyActor = nullptr;

public:

	FName CurrentWarpTargetName;

protected:
	
	UCombatTracingComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	AActor* TraceForEnemyActor(FCombatTraceData TraceData);

	UFUNCTION(BlueprintCallable)
	void RemoveEnemyActor();
	
	float GetDistanceFromActorToAxis(AActor* ActorToCheck, FVector TraceStartLocation, FVector TraceAxis);

	AActor* FilterClosestActorToAxisTrace(TArray<FHitResult> HitArray, FVector TraceStartLocation, FVector TraceEndLocation);
	AActor* FilterClosestActorToOwner(TArray<FHitResult> HitArray);
	
	FVector GetTraceDirection();
};
