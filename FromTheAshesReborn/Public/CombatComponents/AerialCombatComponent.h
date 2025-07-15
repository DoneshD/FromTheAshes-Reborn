#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraSystemParams.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AerialCombatComponent.generated.h"


class UCharacterMovementComponent;
class UFTAAbilitySystemComponent;
class AFTACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UAerialCombatComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	int32 PreviousCount;

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTACharacter;

	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> FTAAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CMC;
	

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic Tags")
	FGameplayTag EnableTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic Tags")
	FGameplayTag AerialAttackCounterTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	FCameraSystemParams CameraParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	float TimeGravityMultiplier = 0.65f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	float LaunchStrengthMultiplier = 1.0f;

	bool IsComponentActive = false;
	int AttackCounter = 0;

	float AttackLastResetTime;
	float AttackTimeInAir = 0.0f;

	float TotalAirTime = 0.0f;

	float LaunchStrength = 100.0f;

protected:

	void ClearStateAndVariables();
	void InitializeStateAndVariables();
	void EnableComponent(const FGameplayTag InEnableTag, int32 NewCount);
	
	void AddAttackCounterTag(const FGameplayTag InAttackCounterTag, int32 NewCount);
	
	float CalculateTimeSpentGravityMultiplier() const;
	void ResetAttackTimer();
	float GetAttackElapsedTime() const;

public:
	
	UAerialCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetGravity(float NewGravity);
	void PrintGravity();
	
	float CalculateAttackAntiGravityMultiplier(int InNewCount);
	
};
