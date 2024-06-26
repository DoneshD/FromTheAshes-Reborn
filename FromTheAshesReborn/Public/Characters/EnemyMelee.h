#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "Interfaces/MeleeCombatantInterface.h"
#include "EnemyMelee.generated.h"

class UMeleeAttackLogicComponent;
class AMeleeWeapon;

UCLASS()
class FROMTHEASHESREBORN_API AEnemyMelee : public AEnemyBase, public IMeleeCombatantInterface
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMeleeAttackLogicComponent> MeleeAttackLogicComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Attack")
	TObjectPtr<UAnimMontage> LightAttackAnim;

	UPROPERTY(VisibleAnywhere)
	AMeleeWeapon* MeleeWeapon_L;

	UPROPERTY(VisibleAnywhere)
	AMeleeWeapon* MeleeWeapon_R;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AMeleeWeapon> MeleeWeaponClass;


public:
	AEnemyMelee(const FObjectInitializer& object_initializer);

	virtual void BeginPlay() override;

	//Enemy Interface functions

	virtual void GetIdealRange(float& OutAttackRadius, float& OutDefendRadius, float& OutOrbitRadius) override;

	virtual float SetMovementSpeed(EMovementSpeed SpeedState) override;

	virtual void LightAttack() override;

	//Melee Combatant Interface functions
	
	UFUNCTION()
	virtual AMeleeWeapon* GetLeftWeapon() override;

	UFUNCTION()
	virtual AMeleeWeapon* GetRightWeapon() override;

	UFUNCTION()
	virtual void EmptyHitActorsArray() override;

	UFUNCTION(BlueprintCallable)
	virtual void StartMeleeAttackCollisions() override;

	UFUNCTION(BlueprintCallable)
	virtual void EndMeleeAttackCollisions() override;

	UFUNCTION()
	virtual bool MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits) override;

	UFUNCTION()
	virtual void MeleeTraceCollisions() override;

	UFUNCTION(BlueprintCallable)
	virtual void MeleeAttackWarpToTarget() override;

	UFUNCTION(BlueprintCallable)
	virtual void ResetMeleeAttackWarpToTarget() override;

	

};
