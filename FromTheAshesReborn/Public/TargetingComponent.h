#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"

class APlayableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	TObjectPtr<APlayableCharacter> PlayableCharacter;


	bool IsTargeting = false;
	FVector TargetRotateLocation;

	TObjectPtr<AActor> HardTarget;
	TObjectPtr<AActor> SoftTarget;

	AActor* TeleportTarget;


private:

public:	
	
	UTargetingComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SoftLockOn(float Distance);

	void HardLockOn();

	void StartTeleport();
		
};
