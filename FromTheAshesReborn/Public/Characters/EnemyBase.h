#pragma once

#include "CoreMinimal.h"
#include "FTACharacter.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "AIController.h"
#include "Interfaces/AIEnemyInterface.h"
#include "Interfaces/DamagableInterface.h"
#include "../Interfaces/AttackTokenSystemInterface.h"
#include "EventManagers/CombatManager.h"
#include "EnemyBase.generated.h"

DECLARE_DELEGATE(FOnAttackEnd);

class AIControllerEnemyBase;
class UArrowComponent;
class AFromTheAshesRebornGameMode;

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBase : public AFTACharacter, public IDamagableInterface, public IAIEnemyInterface, public IAttackTokenSystemInterface
{
	GENERATED_BODY()

public:

	TObjectPtr<AFromTheAshesRebornGameMode> FTAGameMode;

	AAIControllerEnemyBase* AICEnemyBase;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BehaviorTree;

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

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Hit Reactions")
	TObjectPtr<UAnimMontage> BaseAttack;

public:

	AEnemyBase();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void FinishLightMeleeAttack();


	//Damagable Interface functions

	UFUNCTION()
	virtual void HandleDeath() override;

	UFUNCTION()
	virtual void HandleHitReaction(FDamageInfo DamageInfo) override;


	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHealth() override;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsDead() override;

	UFUNCTION(BlueprintCallable)
	virtual float Heal(float NewHeatlh) override;

	UFUNCTION(BlueprintCallable)
	virtual bool TakeDamage(FDamageInfo DamageInfo) override;


	//Enemy Interface functions

	UFUNCTION()
	virtual float SetMovementSpeed(EMovementSpeed SpeedState) override;

	UFUNCTION(BlueprintCallable)
	virtual void GetIdealRange(float& OutAttackRadius, float& OutDefendRadius, float& OutOrbitRadius) override;

	UFUNCTION(BlueprintCallable)
	virtual void JumpToDestination(FVector Destination) override;

	UFUNCTION(BlueprintCallable)
	virtual void LightAttack() override;


	//Attacker Interface 

	virtual void Attack(TObjectPtr<AActor> AttackTarget) override;

	virtual void Orbit(TObjectPtr<AActor> AttackTarget) override;

	virtual void Retreat() override;

	//Attack Token System Interface

	UFUNCTION()
	virtual bool AttackStart(AActor* AttackTarget, int TokensNeeded) override;

	UFUNCTION()
	virtual void AttackEnd(AActor* AttackTarget) override;

	UFUNCTION()
	virtual bool ReserveAttackToken(int Amount) override;

	UFUNCTION()
	virtual void ReturnAttackToken(int Amount) override;

	UFUNCTION()
	virtual void StoreAttackTokens(AActor* AttackTarget, int Amount) override;

	
	void FunctionToExecuteOnAnimationBlendOut(UAnimMontage* animMontage, bool bInterrupted);

	void FunctionToExecuteOnAnimationEnd(UAnimMontage* animMontage, bool bInterrupted);

	void PlayAnAnimationMontage(UAnimMontage* montageToPlay);
	
};
