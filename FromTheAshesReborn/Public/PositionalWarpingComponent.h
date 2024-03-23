#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PositionalWarpingComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UPositionalWarpingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	TObjectPtr<UArrowComponent> CurrentWarpPositionalArrow;

	struct ArrowNeighbors
	{
		TObjectPtr<UArrowComponent> RightNeighbor;
		TObjectPtr<UArrowComponent> LeftNeighbor;
	};

	TMap<TObjectPtr<UArrowComponent>, ArrowNeighbors> ArrowNeighborMap;

protected:

public:	

	UPositionalWarpingComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void InitializeArrowNeighbors();

	TObjectPtr<UArrowComponent> GetLeftArrowNeighbor(TObjectPtr<UArrowComponent> Arrow);

	TObjectPtr<UArrowComponent> GetRightArrowNeighbor(TObjectPtr<UArrowComponent> Arrow);

	UFUNCTION()
	void UpdateWarpTargetPostion(FMotionWarpingTarget MotionWarpingTargetParams);

	UFUNCTION()
	void ResetWarpTargetPostion(FName TargetName);

	TObjectPtr<UArrowComponent> GetPositionalArrow(EFacingDirection FacingDirection);

	UFUNCTION()
	EFacingDirection GetFacingDirection(FVector FacingLocation);

};
