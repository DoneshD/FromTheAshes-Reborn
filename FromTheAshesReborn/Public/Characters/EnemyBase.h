#pragma once

#include "CoreMinimal.h"
#include "FTACharacter.h"
#include "AI/Controllers/AIControllerEnemyBase.h"
#include "AIController.h"
#include "Interfaces/AIEnemyInterface.h"
#include "Interfaces/DamagableInterface.h"
#include "Interfaces/MotionWarpingInterface.h"
#include "EnemyBase.generated.h"

DECLARE_DELEGATE(FOnAttackEnd);

class AIControllerEnemyBase;
class UArrowComponent;

UCLASS()
class FROMTHEASHESREBORN_API AEnemyBase : public AFTACharacter, public IDamagableInterface, public IAIEnemyInterface, public IMotionWarpingInterface
{
	GENERATED_BODY()

public:
	AAIControllerEnemyBase* AICEnemyBase;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> BehaviorTree;

	TMap<AActor*, int> ReservedAttackTokensMap;

	int TokensUsedInCurrentAttack;

	FOnAttackEnd OnAttackEnd;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	TObjectPtr<UMaterialInterface> TargetedMaterial;

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
	TObjectPtr<UAnimMontage> DeathReaction;

protected:
	virtual void BeginPlay() override;

private:

public:
	AEnemyBase();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void FinishLightMeleeAttack();

	void HandleDeathReaction();

	void HandleHitReaction(FDamageInfo DamageInfo);

	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	void OnMontageInterrupted(UAnimMontage* Montage, bool bInterrupted);

	//Damagable Interface functions

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

	UFUNCTION(BlueprintCallable)
	virtual bool ReserveAttackToken(int Amount) override;

	UFUNCTION(BlueprintCallable)
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

	UFUNCTION()
	virtual void OnTargeted() override;

	UFUNCTION()
	virtual void EndTargeted() override;


	//Motion Warping Interface

	UFUNCTION(BlueprintCallable)
	virtual void UpdateWarpTarget(FMotionWarpingTarget& MotionWarpingTargetParams) override;

	UFUNCTION()
	virtual void ResetWarpTarget() override;

	virtual TObjectPtr<UArrowComponent> GetPositionArrow(EHitDirection HitDirection) override;

	UFUNCTION()
	virtual EHitDirection GetHitEnemyDirection(FVector HitLocation) override;

};
