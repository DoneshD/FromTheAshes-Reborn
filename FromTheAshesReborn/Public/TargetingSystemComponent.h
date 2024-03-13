#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UTargetingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	TObjectPtr<APlayableCharacter> PlayableCharacter;

	bool IsTargeting = false;
	FVector TargetRotateLocation;

	TObjectPtr<AActor> HardTarget;
	TObjectPtr<AActor> SoftTarget;


protected:

public:	

	UTargetingSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void HardLockOn();
		
};
