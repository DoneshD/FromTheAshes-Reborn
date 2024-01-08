// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/EStates.h"
#include "DamageSystem/DamageSystem.h"
#include "AttacksComponent.h"
#include "FTACharacter.generated.h"

class UArrowComponent;

UCLASS()
class FROMTHEASHESREBORN_API AFTACharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> FrontArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> BackArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> LeftArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UArrowComponent> RightArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DamageSystem", meta = (AllowPrivateAccess = "true"))
	class UDamageSystem* DamageSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attacks", meta = (AllowPrivateAccess = "true"))
	class UAttacksComponent* AttacksComponent;

	//FSM checks
	bool CanJump();
	bool bCanDodge;

	FVector2D InputDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	int JumpCount = 0;

	//FSM functions
	EStates GetState() const;
	void SetState(EStates NewState);
	bool IsStateEqualToAny(TArray<EStates> StatesToCheck);

public:
	AFTACharacter();

	virtual void Tick(float DeltaTime) override;

	EStates CurrentState;


	
};
