#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashSystemComponent.generated.h"

class UArrowComponent;
class UCharacterMovementComponent;
class AFromTheAshesRebornGameMode;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UDashSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	TObjectPtr<AFromTheAshesRebornGameMode> FTAGameMode;

	TObjectPtr<UArrowComponent> DashWarpTargetArrow;

	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Anim")
	TObjectPtr<UAnimMontage> CurrentDashAnim;

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

	bool EnableLateralDash = false;

	bool IsDashSaved = false;

	bool CanDashAttack = false;

protected:

public:	

	float GetAngleOfDash();

	bool InRangeOfLateralDash();

	void SelectBlink();

	UFUNCTION(BlueprintCallable)
	void PerformDash(bool HasInput);

	void DashWarpToTarget(bool HasInput);

	void ResetDashWarpToTarget();

	UDashSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
