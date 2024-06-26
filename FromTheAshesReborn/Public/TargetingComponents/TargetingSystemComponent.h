#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingSystemComponent.generated.h"

class AFromTheAshesRebornGameMode;
class APlayableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UTargetingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	TObjectPtr<AFromTheAshesRebornGameMode> FTAGameMode;

	TObjectPtr<APlayableCharacter> PlayableCharacter;

	UPROPERTY(BlueprintReadWrite)
	bool IsTargeting = false;

	FVector TargetRotateLocation;

	TObjectPtr<AActor> SoftTarget;

	FVector MidPoint;

	float Radius;

public:	

	UTargetingSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void HardLockOn();

	void DisableLockOn();

	FVector CalculateMidpoint(FVector PlayerLocation, FVector TargetLocation);

	float CalculateDistance(FVector PlayerLocation, FVector TargetLocation);
		
};
