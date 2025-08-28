#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "CentralStateComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCentralStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag GroundedTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Grounded");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AirborneTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Airborne");

protected:
	
	UCentralStateComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	
};
