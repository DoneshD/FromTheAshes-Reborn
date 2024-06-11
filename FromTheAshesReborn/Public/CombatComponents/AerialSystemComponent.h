#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AerialSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UAerialSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	


public:	
	UAerialSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	virtual bool JumpLineTrace(FVector StartLocation, FVector EndLocation);

	UFUNCTION(BlueprintCallable)
	virtual void JumpWarpToTarget();

	UFUNCTION(BlueprintCallable)
	virtual void ResetJumpWarpToTarget();
};
