#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AerialCombatComponent.generated.h"


class UDownedCombatComponent;
class UGameplayEffect;
class UCharacterMovementComponent;
class UFTAAbilitySystemComponent;
class AFTACharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UAerialCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	bool ActivateFromLauncher = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gravity")
	TObjectPtr<UCurveFloat> GravityCurve;
	

private:
	int32 PreviousCount;

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTACharacter;

	UPROPERTY()
	TObjectPtr<UFTAAbilitySystemComponent> FTAAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CMC;

	float LastGravityCurveValue = 0.0f;

protected:

	bool IsComponentActive = false;
	
	FGameplayTag EnableTag = FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent");
	

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE")
	TSubclassOf<UGameplayEffect> EnableAerialCombatEffect;

	int TestAttackCounter = 0;
	float TotalAirTime = 0.0f;

protected:

	void ClearStateAndVariables();
	void InitializeStateAndVariables();
	
	void EnableComponent(const FGameplayTag InEnableTag, int32 NewCount);
	
	void DisableCollision();
	void EnableCollision();

public:
	
	UAerialCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetGravity(float NewGravity);
	void PrintGravity();
	
	void AbilityInitTest();
	
};
