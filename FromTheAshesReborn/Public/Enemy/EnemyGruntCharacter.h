#pragma once

#include "CoreMinimal.h"
#include "EnemyBaseCharacter.h"
#include "EnemyGruntCharacter.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AEnemyGruntCharacter : public AEnemyBaseCharacter
{
	GENERATED_BODY()

protected:
	
	AEnemyGruntCharacter(const class FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
