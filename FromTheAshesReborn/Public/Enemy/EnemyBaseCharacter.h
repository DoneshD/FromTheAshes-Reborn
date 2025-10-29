#pragma once

#include "CoreMinimal.h"
#include "FTACustomBase/FTACharacter.h"
#include "EnemyBaseCharacter.generated.h"

class UWeaponDefinition;
class UWidgetComponent;

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBaseCharacter : public AFTACharacter
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UWidgetComponent> HealthWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
	TSubclassOf<UWeaponDefinition> WeaponClass;

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
