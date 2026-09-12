#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DismembermentComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UDismembermentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FName HitBoneName;


protected:
	
	UDismembermentComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CaptureDismembermentData(FName InBoneName);

	void RenameBoneName();

};
