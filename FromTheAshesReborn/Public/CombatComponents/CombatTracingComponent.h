#pragma once

#include "CoreMinimal.h"
#include "MeleeWarpingComponent.h"
#include "Components/ActorComponent.h"
#include "CombatTracingComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCombatTracingComponent : public UActorComponent
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
	UCombatTracingComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetDistanceFromActorToAxis(AActor* ActorToCheck, FVector TraceStartLocation, FVector TraceAxis);

	AActor* FilterClosestActorToAxisTrace(TArray<FHitResult> HitArray, FVector TraceStartLocation, FVector TraceEndLocation);
	AActor* FilterClosestActorToOwner(TArray<FHitResult> HitArray);
	
	FVector GetTraceDirection();
};
