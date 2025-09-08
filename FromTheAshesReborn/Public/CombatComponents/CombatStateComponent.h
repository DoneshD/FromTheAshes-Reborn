#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "CombatStateComponent.generated.h"

class UGameplayEffect;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UCombatStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag GroundedTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Grounded");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AirborneTag = FGameplayTag::RequestGameplayTag("Character.Orientation.Airborne");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag NeutralTag = FGameplayTag::RequestGameplayTag("Character.State.Neutral");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag DownedTag = FGameplayTag::RequestGameplayTag("Character.State.Downed");

protected:
	UCombatStateComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
