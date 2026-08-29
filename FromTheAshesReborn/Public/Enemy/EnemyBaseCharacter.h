#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "EnemyBaseCharacter.generated.h"

class AEnemyBaseCharacter;
class USphereComponent;
class UEnemyCharacterDataAsset;
class UStateTree;
class UAICombatParameters;
class UWeaponDefinition;
class UWidgetComponent;

DECLARE_MULTICAST_DELEGATE(FOnEnemyDeathSignature);

UENUM(BlueprintType)
enum class EEnemyEngagementRole : uint8
{
	None		UMETA(DisplayName = "None"),
	Aggressor	UMETA(DisplayName = "Aggressor"),
	Cover		UMETA(DisplayName = "Cover"),
	Observer	UMETA(DisplayName = "Observer")
};

DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnPlayerInAggressionRadius,
	TObjectPtr<AEnemyBaseCharacter>,
	EEnemyEngagementRole
);

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBaseCharacter : public AFTACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAICombatParameters> AICombatParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UEnemyCharacterDataAsset> EnemyDataAsset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UWidgetComponent> HealthWidget;

	FOnEnemyDeathSignature OnDeath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat")
	TObjectPtr<USphereComponent> PlayerDetectionSphere;

	FOnPlayerInAggressionRadius OnPlayerInAggressionRadius;


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
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent*OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HealthChanged(UHealthComponent* InHealthComponent, float OldValue, float NewValue, AActor* InInstigator);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(UHealthComponent* InHealthComponent, float OldValue, float NewValue, AActor* InInstigator);

	UFUNCTION(BlueprintCallable)
	void CheckDeath(float NewValue);

	void Death();

};
