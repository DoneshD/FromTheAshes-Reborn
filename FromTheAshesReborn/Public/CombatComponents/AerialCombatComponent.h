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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE")
	TSubclassOf<UGameplayEffect> AddAerialCombatGravity;

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

protected:

	bool IsComponentActive = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic Tags")
	FGameplayTag EnableTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic Tags")
	FGameplayTag AerialAttackCounterTag;
	
	float TotalAirTime = 0.0f;


public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GE")
	TSubclassOf<UGameplayEffect> EnableAerialCombatEffect;


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
	
	
};
