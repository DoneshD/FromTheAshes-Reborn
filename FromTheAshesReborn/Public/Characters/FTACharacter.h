// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/EStates.h"
#include "MotionWarpingComponent.h"
#include "FTACharacter.generated.h"

class UArrowComponent;
class UDamageSystem;
class UAttacksComponent;

UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DamageSystemComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDamageSystem> DamageSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AttacksComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAttacksComponent> AttacksComponent;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Jump")
	int JumpCount = 0;

	FVector2D InputDirection;
	EStates CurrentState;

public:

	AFTACharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	EStates GetState() const;

	void SetState(EStates NewState);

	bool IsStateEqualToAny(TArray<EStates> StatesToCheck);

	bool CanJump();

};
