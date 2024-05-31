#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	


public:	

	AEnemySpawner();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
