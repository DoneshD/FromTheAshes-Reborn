#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParkourInterface.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UMotionWarpingComponent;

UINTERFACE()
class UParkourInterface : public UInterface
{
	GENERATED_BODY()
};

class FROMTHEASHESREBORN_API IParkourInterface
{
	GENERATED_BODY()

public:

	virtual bool SetIntializeReference(ACharacter* Character, USpringArmComponent* CameraBoom, UCameraComponent* Camera,
		UMotionWarpingComponent* MotionWarping) = 0;
	
};
