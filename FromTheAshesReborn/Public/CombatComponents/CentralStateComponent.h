#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "CentralStateComponent.generated.h"


class UCharacterMovementComponent;
class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCentralStateComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UPROPERTY()
	FGameplayTag CurrentStateTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	FGameplayTag CurrentOrientationTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CMC;

public:
	
	UPROPERTY()
	FGameplayTag GroundedOrientationTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Grounded");
	
	UPROPERTY()
	FGameplayTag AirborneOrientationTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Airborne");
	
	UPROPERTY()
	FGameplayTag NeutralStateTag = FGameplayTag::RequestGameplayTag("Character.State.Neutral");
	
	UPROPERTY()
	FGameplayTag DownedStateTag = FGameplayTag::RequestGameplayTag("Character.State.Downed");

protected:
	
	UCentralStateComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	void HandleNeutralState();
	void HandleDownedState();
	void HandleGroundedOrientation();
	void HandeAirborneOrientation();

public:
	
	void SetCurrentState(FGameplayTag StateTag);
	void SetCurrentOrientation(FGameplayTag OrientationTag);

	FGameplayTag GetCurrentState();
	FGameplayTag GetCurrentOrientation();
};
