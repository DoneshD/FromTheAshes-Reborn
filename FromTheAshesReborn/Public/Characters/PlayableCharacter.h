// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/MotionWarpingInterface.h"
#include "Interfaces/DamagableInterface.h"
#include "Interfaces/MeleeCombatantInterface.h"
#include "DamageSystem/DamageSystem.h"
#include "DamageSystem/DamageInfo.h"
#include "Interfaces/DashingCombatantInterface.h"
#include "InputAction.h"
#include "PlayableCharacter.generated.h"


class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class UTimelineComponent;
class UCurveFloat;
class UComboSystemComponent;
class UMeleeAttackLogicComponent;
class UDashSystemComponent;
class UTargetingSystemComponent;

UCLASS()
class FROMTHEASHESREBORN_API APlayableCharacter : public AFTACharacter, public IDamagableInterface, public IMeleeCombatantInterface, public IDashingCombatantInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UComboSystemComponent> ComboSystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMeleeAttackLogicComponent> MeleeAttackLogicComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDashSystemComponent> DashSystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTargetingSystemComponent> TargetingSystemComponent;

	//-----------------------------------------Inputs-----------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultInputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_LookMouse;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_LookStick;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_LightAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_HeavyAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_Dash;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_LockOn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> Input_SlowTime;

	//-----------------------------------------Light Attack-----------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasMovementInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsIdleCombat = false;

	int LightAttackIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsLightAttackSaved;

	//-----------------------------------------Heavy Attack-----------------------------------------

	int HeavyAttackIndex = 0;
	int NewHeavyAttackIndex = 0;
	bool IsHeavyAttackSaved;
	bool IsHeavyAttackPaused = false;
	bool IsSurgeAttackPaused = false;

	//-----------------------------------------Combo Strings---------------------------------------
	int ComboExtenderIndex = 0;

	int ComboSurgeCount = 0;
	float ComboSurgeSpeed = 1.0;
	bool IsSurging = false;

	bool BranchFinisher = false;


	//-----------------------------------------Lock Ons--------------------------------------------

	bool IsTargeting = false;
	FVector TargetRotateLocation;

	TObjectPtr<AActor> HardTarget;
	TObjectPtr<AActor> SoftTarget;

	//-----------------------------------------Timers----------------------------------------------

	FTimerHandle HeavyAttackPauseHandle;
	FTimerHandle SurgeAttackPauseHandle;

	FTimerHandle IdleCombatHandle;

	//-----------------------------------------Timelines-------------------------------------------

	TObjectPtr<UTimelineComponent> RotationTimeline;

	TObjectPtr<UTimelineComponent> TeleportTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	TObjectPtr<UCurveFloat> BufferCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	TObjectPtr<UCurveFloat> RotationCurve;

	UPROPERTY(EditAnywhere, Category = "Timeline")
	TObjectPtr<UCurveFloat> TeleportCurve;

	FOnTimelineFloat InterpFunction{};
	FOnTimelineEvent TimelineFinished{};

	float BufferAmount;

	//-----------------------------------------Anim Montages---------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> LightAttackCombo;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> HeavyAttackCombo;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboExtender;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboExtenderFinishers;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> PausedHeavyAttackCombo1;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TObjectPtr<UAnimMontage> ComboSurge_L;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TObjectPtr<UAnimMontage> ComboSurge_R;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TObjectPtr<UAnimMontage> ComboBybass;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> ForwardDashAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> BackwardDashAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> RightDashAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> LeftDashAnim;


	bool IsSlowTime = false;

private:

public:
	APlayableCharacter();

	virtual void BeginPlay() override;

	void InputSlowTime();

	//-----------------------------------------FSM Reset States-------------------------------------

	UFUNCTION(BlueprintCallable, Category = "FSM")
	void ResetState();

	void ResetLightAttack();
	void ResetHeavyAttack();
	void ResetCombos();
	void ResetSurgeCombo();

	//-----------------------------------------Movement---------------------------------------------

	void Move(const FInputActionInstance& Instance);
	void MoveCanceled();
	void LookMouse(const FInputActionValue& InputValue);
	void LookStick(const FInputActionValue& InputValue);

	void DoubleJump();
	void StopJump();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void EnableRootRotation();

	void InputDash();
	bool CanDash();
	bool CanJump();

	//-----------------------------------------FSM Attack Check-------------------------------------

	bool CanAttack();
	//-----------------------------------------Light Attacks----------------------------------------

	void InputLightAttack();
	void LightAttack();
	void PerformLightAttack(int LightAttackIndex);

	UFUNCTION(BlueprintCallable, Category = "Light Attack")
	void SaveLightAttack();

	//-----------------------------------------Heavy Attacks----------------------------------------

	void InputHeavyAttack();
	void HeavyAttack();
	void PerformHeavyAttack(int HeavyAttackIndex);

	UFUNCTION(BlueprintCallable, Category = "Heavy Attack")
	void SaveHeavyAttack();

	//-----------------------------------------Pause Combos-----------------------------------------

	void HeavyAttackPaused();
	void SelectHeavyPauseCombo();
	void NewHeavyCombo();
	void PerformHeavyPauseCombo(TArray<TObjectPtr<UAnimMontage>> PausedHeavyAttackCombo);

	void SurgeAttackPaused();

	void StartHeavyAttackPausedTimer();
	void ClearHeavyAttackPausedTimer();

	void StartIdleCombatTimer();
	void ClearIdleCombatTimer();

	void StartSurgeAttackPausedTimer();
	void ClearSurgeAttackPausedTimer();

	//-----------------------------------------Attack Strings---------------------------------------

	void PerformComboExtender(int ComboExtenderIndex);
	void PerformComboFinisher(UAnimMontage* FinisherMontage);
	void PerformComboSurge();


	//-----------------------------------------LockOn----------------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Lock On")
	void RotationToTarget();
	void StopRotation();

	void SoftLockOn(float Distance);

	void InputLockOn();

	//-----------------------------------------Timelines--------------------------------------------

	UFUNCTION()
	void TimelineFloatReturn(float value);

	UFUNCTION()
	void OnTimelineFinished();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

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

	//Melee Combatant Interface

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

	//Dashing Combatant Interface

	UFUNCTION(BlueprintCallable)
	virtual void DashWarpToTarget() override;

	UFUNCTION(BlueprintCallable)
	virtual void ResetDashWarpToTarget() override;

};