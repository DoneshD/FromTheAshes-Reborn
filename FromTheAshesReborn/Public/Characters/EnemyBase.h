// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FTACharacter.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "AIController.h"
#include "Interfaces/AIEnemyInterface.h"
#include "Interfaces/DamagableInterface.h"
#include "EnemyBase.generated.h"

DECLARE_DELEGATE(FOnAttackEnd);

class AIControllerEnemyBase;

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBase : public AFTACharacter, public IDamagableInterface, public IAIEnemyInterface
{
	GENERATED_BODY()

public:
	AAIControllerEnemyBase* AICEnemyBase;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	TMap<AActor*, int> ReservedAttackTokensMap;

	int TokensUsedInCurrentAttack;

	FOnAttackEnd OnAttackEnd;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> LeftHitReaction;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> RightHitReaction;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> FrontHitReaction;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> BackHitReaction;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> KnockbackHitReaction;

protected:
	virtual void BeginPlay() override;

private:

public:
	AEnemyBase();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void FinishLightMeleeAttack();

	void HandleHitReaction(FDamageInfo DamageInfo);

	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	void OnMontageInterrupted(UAnimMontage* Montage, bool bInterrupted);

	//Damagable Interface functions

	UFUNCTION()
	virtual float GetCurrentHealth() override;

	UFUNCTION()
	virtual float GetMaxHealth() override;

	UFUNCTION()
	virtual bool IsDead() override;

	UFUNCTION()
	virtual float Heal(float NewHeatlh) override;

	UFUNCTION()
	virtual bool TakeDamage(FDamageInfo DamageInfo) override;

	UFUNCTION()
	virtual bool ReserveAttackToken(int Amount) override;

	UFUNCTION()
	virtual void ReturnAttackToken(int Amount) override;

	//Enemy Interface functions

	UFUNCTION()
	virtual float SetMovementSpeed(EMovementSpeed SpeedState) override;

	UFUNCTION(BlueprintCallable)
	virtual void GetIdealRange(float& OutAttackRadius, float& OutDefendRadius) override;

	UFUNCTION(BlueprintCallable)
	virtual void JumpToDestination(FVector Destination) override;

	UFUNCTION(BlueprintCallable)
	virtual void LightAttack() override;

	UFUNCTION()
	virtual bool AttackStart(AActor* AttackTarget, int TokensNeeded) override;

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd(AActor* AttackTarget) override;

	UFUNCTION()
	virtual void StoreAttackTokens(AActor* AttackTarget, int Amount) override;

};
