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

	UPROPERTY()
	FGameplayTag GroundedTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Grounded");
	
	UPROPERTY()
	FGameplayTag AirborneTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Airborne");
	
	UPROPERTY()
	FGameplayTag NeutralTag = FGameplayTag::RequestGameplayTag("Character.State.Neutral");
	
	UPROPERTY()
	FGameplayTag DownedTag = FGameplayTag::RequestGameplayTag("Character.State.Downed");

protected:
	
	UCentralStateComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
