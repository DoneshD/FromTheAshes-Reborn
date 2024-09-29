#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GroundedMeleeComboComponent.generated.h"

class UMeleeAttackDataAsset;
class UAbilitySystemComponent;
class UFTAAT_PlayMontageAndWaitForEvent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UGroundedMeleeComboComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UGroundedMeleeComboComponent();

	FGameplayTagContainer CurrentComboTagContainer;
	int CurrentComboIndex = 0;

	UAbilitySystemComponent* AbilitySystemComponent;

public:
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrintCurrentComboContainer();

	FGameplayTagContainer& GetCurrentComboContainer();

	int GetCurrentComboIndex();

	void SetCurrentComboIndex(int Index);
};
