#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboSystemComponent.generated.h"

class APlayableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UComboSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	TObjectPtr<APlayableCharacter> PC;

protected:

public:	

	UComboSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SaveLightAttack();

	void SaveHeavyAttack();
	
	void PerformCurrentAttack(float WarpingDistance, TObjectPtr<UAnimMontage> CurrentAttackMontage);

	void PerformLightAttack(int LightAttackIndex);

	void PerformHeavyAttack(int HeavyAttackIndex);

	void PerformHeavyPauseCombo(TArray<TObjectPtr<UAnimMontage>> PausedHeavyAttackCombo);

	void PerformComboExtender(int ExtenderIndex);

	void PerformComboFinisher(TObjectPtr<UAnimMontage> FinisherMontage);

	void PerformComboSurge();

};
