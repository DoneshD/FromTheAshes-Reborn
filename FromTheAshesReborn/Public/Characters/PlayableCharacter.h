#pragma once

#include "CoreMinimal.h"
#include "FTACharacter.h"
#include "Interfaces/DashingCombatantInterface.h"
#include "Interfaces/AttackTargetInterface.h"
#include "Interfaces/DamagableInterface.h"
#include "Interfaces/MeleeCombatantInterface.h"
//#include "Interfaces/PlayerCharacterInterface.h"
#include "../Interfaces/PlayerCharacterInterface.h"
#include "../Interfaces/AttackTokenSystemInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/TimelineComponent.h"
#include "DamageSystem/DamageSystem.h"
#include "DamageSystem/DamageInfo.h"
#include "Weapons/MeleeWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
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
class UGroundedComboStringComponent;
class UNiagaraSystem;

UCLASS()
class FROMTHEASHESREBORN_API APlayableCharacter : public AFTACharacter, public IDamagableInterface, public IMeleeCombatantInterface, 
	public IDashingCombatantInterface, public IAttackTargetInterface, public IPlayerCharacterInterface, public IAttackTokenSystemInterface
{
	GENERATED_BODY()

public:

	TObjectPtr<AFromTheAshesRebornGameMode> FTAGameMode;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> LockOnSphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UGroundedComboStringComponent> GroundedComboStringComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMeleeAttackLogicComponent> MeleeAttackLogicComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDashSystemComponent> DashSystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTargetingSystemComponent> TargetingSystemComponent;

	UPROPERTY(VisibleAnywhere)
	AMeleeWeapon* MeleeWeapon_L;

	UPROPERTY(VisibleAnywhere)
	AMeleeWeapon* MeleeWeapon_R;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AMeleeWeapon> LightMeleeWeaponClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AMeleeWeapon> HeavyMeleeWeaponClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Niagara")
	UNiagaraSystem* DashEffect;



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
	bool IsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsIdleCombat = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DisableRootMotion = false;


	//-----------------------------------------Lock Ons--------------------------------------------

	bool IsTargeting = false;
	FVector TargetRotateLocation;

	TObjectPtr<AActor> HardTarget;
	TObjectPtr<AActor> SoftTarget;

	FVector InitialSphereLocation;

	//-----------------------------------------Timers----------------------------------------------

	FTimerHandle HeavyAttackPauseHandle;
	FTimerHandle SurgeAttackPauseHandle;

	FTimerHandle IdleCombatHandle;
	FTimerHandle IsSprintingTimerHandle;

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

	UPROPERTY(EditAnywhere)
	int MaxAttackersCount = 1;

	//-----------------------------------------Anim Montages---------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> LightAttackCombo;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> HeavyAttackCombo;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboSeq3;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboSeq4;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboSeq5;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboSeq6;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboSeq7;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboSeq8;
	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboSeq9;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TArray<TObjectPtr<UAnimMontage>> ComboSeq10;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TObjectPtr<UAnimMontage> ForwardLightDashAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Anim")
	TObjectPtr<UAnimMontage> ForwardHeavyDashAttack;

	//-----------------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> ForwardDashAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> BackwardDashAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> RightDashAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> LeftDashAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> ForwardBlinkAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> BackwardBlinkAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> RightBlinkAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> LeftBlinkAnim;

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

	UPROPERTY(EditDefaultsOnly, Category = "Reaction Anim")
	TObjectPtr<UAnimMontage> DeathMontage;

	bool IsSlowTime = false;

private:

public:
	APlayableCharacter();

	virtual void BeginPlay() override;

	void InputSlowTime();

	//-----------------------------------------FSM Reset States-------------------------------------

	UFUNCTION(BlueprintCallable, Category = "FSM")
	void ResetState();

	void ResetComboString();

	//-----------------------------------------Movement---------------------------------------------

	void Move(const FInputActionInstance& Instance);
	void StartMove();
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

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SaveDash();

	//-----------------------------------------FSM Attack Check-------------------------------------

	bool CanAttack();

	void InputLightAttack();
	void InputHeavyAttack();

	//-----------------------------------------Heavy Attacks----------------------------------------

	UFUNCTION(BlueprintCallable, Category = "Heavy Attack")
	void SpawnLeftWeapon();

	UFUNCTION(BlueprintCallable, Category = "Heavy Attack")
	void DestroyLeftWeapon();

	//-----------------------------------------Pause Combos-----------------------------------------

	void StartSprintTimer();
	void ClearSprintTimer();

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

	//Player Character Interfaces Functions

	virtual bool GetHasMovementInput() override;

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
	virtual void HandleDeath() override;

	UFUNCTION()
	virtual void HandleHitReaction(FDamageInfo DamageInfo) override;

	//Melee Combatant Interface

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

	//Dashing Combatant Interface

	UFUNCTION(BlueprintCallable)
	virtual void DashWarpToTarget(bool HasInput) override;

	UFUNCTION(BlueprintCallable)
	virtual void ResetDashWarpToTarget() override;

	//Attack Target Interface

	virtual int GetMaxAttackersCount() override;

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

};