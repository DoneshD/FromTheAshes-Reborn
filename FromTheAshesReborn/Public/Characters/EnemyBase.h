// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FTACharacter.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "AIController.h"
#include "Interfaces/AIEnemyInterface.h"
#include "Interfaces/DamagableInterface.h"
#include "EnemyBase.generated.h"

/**
 * 
 */

class AIControllerEnemyBase;

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBase : public AFTACharacter, public IDamagableInterface, public IAIEnemyInterface
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:

public:
	AAIControllerEnemyBase* AICEnemyBase;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	//UPROPERTY(EditAnywhere)
	//TObjectPtr<USkeletalMeshComponent> ParryMesh;

	AEnemyBase();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Damagable Interface functions

	UFUNCTION()
	virtual float NativeGetCurrentHealth() override;

	UFUNCTION()
	virtual float NativeGetMaxHealth() override;

	UFUNCTION()
	virtual bool NativeIsDead() override;

	UFUNCTION()
	virtual float NativeHeal(float NewHeatlh) override;

	UFUNCTION()
	virtual bool NativeTakeDamage(FDamageInfo DamageInfo) override;

	UFUNCTION()
	virtual bool ReserveAttackToken(int Amount) override;

	UFUNCTION()
	virtual void ReturnAttackToken(int Amount) override;

	//Enemy Interface functions

	UFUNCTION()
	virtual float NativeSetMovementSpeed(EMovementSpeed SpeedState) override;

	UFUNCTION(BlueprintCallable)
	virtual void NativeGetIdealRange(float& OutAttackRadius, float& OutDefendRadius) override;

	UFUNCTION(BlueprintCallable)
	virtual void JumpToDestination(FVector Destination) override;

	// Overlap functions
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
