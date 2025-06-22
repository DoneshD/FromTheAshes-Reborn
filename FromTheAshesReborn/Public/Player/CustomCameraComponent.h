#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomCameraComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCustomCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UCustomCameraComponent();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
