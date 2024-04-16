#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "EnemyMelee.generated.h"
#include "Interfaces/MeleeCombatantInterface.h"

class UMeleeAttackLogicComponent;
class AMeleeWeapon;


UCLASS()
class FROMTHEASHESREBORN_API AEnemyMelee : public AEnemyBase, public IMeleeCombatantInterface
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMeleeAttackLogicComponent> MeleeAttackLogicComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack")
	TObjectPtr<UAnimMontage> LightAttackAnim;

private:

public:
	AEnemyMelee();

	virtual void BeginPlay() override;

	//Enemy Interface functions

	virtual void GetIdealRange(float& OutAttackRadius, float& OutDefendRadius) override;

	virtual float SetMovementSpeed(EMovementSpeed SpeedState) override;

	virtual void LightAttack() override;

	//Melee Combatant Interface functions

	UFUNCTION()
	virtual AMeleeWeapon GetLeftWeapon() override;

	UFUNCTION()
	virtual AMeleeWeapon GetRightWeapon() override;

	UFUNCTION()
	virtual void EmptyHitActorsArray() override;

	UFUNCTION()
	virtual void StartMeleeAttackCollisions() override;

	UFUNCTION()
	virtual void EndMeleeAttackCollisions() override;

	UFUNCTION()
	virtual bool MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits) override;

	UFUNCTION()
	virtual void MeleeTraceCollisions() = 0;

	UFUNCTION()
	virtual void MeleeAttackWarpToTarget() = 0;

	UFUNCTION()
	virtual void ResetMeleeAttackWarpToTarget() = 0;


};
