#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/EStates.h"
#include "MotionWarpingComponent.h"
#include "Interfaces/PositionalWarpingInterface.h"
#include "FTACharacter.generated.h"

class UArrowComponent;
class UDamageSystem;
class UPositionalWarpingComponent;
class UAttackTokenSystemComponent;


UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter, public IPositionalWarpingInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DamageSystemComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDamageSystem> DamageSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PositionalWarpingComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPositionalWarpingComponent> PositionalWarpingComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttackTokenSystemComponent> AttackTokenSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> FrontArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> BackArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> LeftArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> RightArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> FrontLeftArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> BackLeftArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> FrontRightArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> BackRightArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> WarpTargetArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Jump")
	int JumpCount = 0;

	FVector2D InputDirection;

	EStates CurrentState;

public:

	AFTACharacter(const FObjectInitializer& object_initializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	EStates GetState() const;

	void SetState(EStates NewState);

	bool IsStateEqualToAny(TArray<EStates> StatesToCheck);

	// IPositionalWarpingInterface

	UFUNCTION()
	virtual void UpdateWarpTargetPostion(FMotionWarpingTarget MotionWarpingTargetParams) override;

	UFUNCTION()
	virtual void ResetWarpTargetPostion(FName TargetName) override;

	UFUNCTION()
	virtual EFacingDirection GetFacingDirection(FVector HitLocation) override;

	virtual TObjectPtr<UArrowComponent> GetPositionalArrow(EFacingDirection HitDirection) override;

	virtual TObjectPtr<UArrowComponent> GetLeftArrowNeighbor(TObjectPtr<UArrowComponent> Arrow) override;

	virtual TObjectPtr<UArrowComponent> GetRightArrowNeighbor(TObjectPtr<UArrowComponent> Arrow) override;
};
