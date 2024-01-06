// Fill out your copyright notice in the Description page of Project Settings.

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


	UPROPERTY(EditDefaultsOnly, Category = "Patrol")
	AActor* PatrolRoute;

private:	

public:	
	APatrolRoute();

	void IncrementPatrolPointIndex();

	FVector GetSplineAsWorldPosition();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
