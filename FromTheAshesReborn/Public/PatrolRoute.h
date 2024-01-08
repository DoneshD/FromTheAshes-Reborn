#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolRoute.generated.h"

class USplineComponent;

UCLASS()
class FROMTHEASHESREBORN_API APatrolRoute : public AActor
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> PatrolRoute;

	int PatrolPointIndex = 0;
	int Direction = 0;

private:	

public:	
	APatrolRoute();

	void IncrementPatrolPointIndex();

	FVector GetSplineAsWorldPosition();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
