// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageSystem/DamageSystem.h"

#include "MeleeAttackLogicComponent.generated.h"

DECLARE_DELEGATE(FOnAttackEnd);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UMeleeAttackLogicComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	UMeleeAttackLogicComponent();

	UDamageSystem* DamageSystemComponent;

	TArray<TObjectPtr<AActor>> AlreadyHitActors_L;
	TArray<TObjectPtr<AActor>> AlreadyHitActors_R;

	TObjectPtr<UArrowComponent> WarpTargetArrow;

	TObjectPtr<AActor> WarpTarget;

	FOnAttackEnd OnAttackEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHitReactionDirection HitReactionDirection;

	bool IsActiveCollision = false;

public:

	void EmptyHitActorsArray();

	UFUNCTION(BlueprintCallable, Category = "Melee Attack")
	void StartMeleeAttackCollisions();

	UFUNCTION(BlueprintCallable, Category = "Melee Attack")
	void EndMeleeAttackCollisions();

	bool MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits);

	void MeleeTraceCollisions();

	void MeleeAttackWarpToTarget(FMotionWarpingTarget& MotionWarpingTargetParams);

	void ResetMeleeAttackWarpToTarget();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
