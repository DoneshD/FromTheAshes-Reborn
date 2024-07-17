#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "EnemyBaseCharacter.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBaseCharacter : public AFTACharacter
{
	GENERATED_BODY()


protected:
	

public:
	AEnemyBaseCharacter();
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
