#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "EnemyBaseCharacter.generated.h"

class UAICombatParameters;
class UWeaponDefinition;
class UWidgetComponent;

DECLARE_MULTICAST_DELEGATE(FOnEnemyDeathSignature);

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBaseCharacter : public AFTACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAICombatParameters> AICombatParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UWidgetComponent> HealthWidget;

	FOnEnemyDeathSignature OnDeath;


public:
	
	UFUNCTION()
	void TimelineProgress(float Value);

	UFUNCTION()
	void TimelineFinished();
	
	bool ShouldRotate = true;

protected:
	
	AEnemyBaseCharacter(const class FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void HealthChanged(UHealthComponent* InHealthComponent, float OldValue, float NewValue, AActor* InInstigator);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(UHealthComponent* InHealthComponent, float OldValue, float NewValue, AActor* InInstigator);

	UFUNCTION(BlueprintCallable)
	void CheckDeath(float NewValue);

	void Death();

};
