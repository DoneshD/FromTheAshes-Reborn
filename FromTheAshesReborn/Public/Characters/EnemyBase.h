// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FTACharacter.h"
#include "AIControllerEnemyBase.h"
#include "AIController.h"
#include "Interfaces/AIEnemyInterface.h"
#include "Interfaces/DamagableInterface.h"
#include "DamageSystem/DamageSystem.h"
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
	AIControllerEnemyBase* AICEnemyBase;

	AEnemyBase();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom", meta = (AllowPrivateAccess = "true"))
	class UDamageSystem* DamageSystemComponent;

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


	//Enemy Interface functions

	UFUNCTION()
	virtual float SetMovementSpeed(EMovementSpeed SpeedState) override;

	UFUNCTION()
	virtual void GetIdealRange(float& OutAttackRadius, float& OutDefendRadius) override;

	
};
