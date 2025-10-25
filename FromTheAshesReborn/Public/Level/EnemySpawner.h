#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "EnemySpawner.generated.h"

class AAIControllerEnemyGrunt;
class AEnemyGruntCharacter;

UCLASS()
class FROMTHEASHESREBORN_API AEnemySpawner : public ATriggerBox
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyGruntCharacter> GruntClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAIControllerEnemyGrunt> GruntControllerClass;

protected:
	
	AEnemySpawner();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
};
