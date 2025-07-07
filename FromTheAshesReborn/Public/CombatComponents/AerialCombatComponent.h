#pragma once

#include "CoreMinimal.h"
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

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic Tags")
	FGameplayTag EnableTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic Tags")
	FGameplayTag AerialAttackCounterTag;
	
	UPROPERTY()
	TObjectPtr<AFTACharacter> FTACharacter;

	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> FTAAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CMC;

	bool IsComponentActive = false;

	int AttackCounter = 0;

	float AttackCounterGravityMultiplier = 0.0f;
	int32 PreviousCount;

	FTimerHandle AerialCombatTimerHandle;
	float ElapsedTime = 0.0f;
	double LastResetTime;
	float TimeInAir = 0.0f;

protected:

	void ClearStateAndVariables();
	void InitializeStateAndVariables();
	void EnableComponent(const FGameplayTag InEnableTag, int32 NewCount);

	void AddAttackCounterTag(const FGameplayTag InAttackCounterTag, int32 NewCount);
	void CalculateTimeSpentGravityMultiplier();
	void ResetTimer();
	float GetElapsedTime() const;

public:
	
	UAerialCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetGravity(float NewGravity);
	void PrintGravity();
	float CalculateAttackCountGravityMultiplier(int InNewCount);
	
};
