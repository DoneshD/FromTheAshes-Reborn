#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageSystem/DamageSystem.h"
#include "Enums/EMeleeAttackRange.h"
#include "MeleeAttackLogicComponent.generated.h"

DECLARE_DELEGATE(FOnAttackEnd);

class AFromTheAshesRebornGameMode;
class APlayableCharacter;
class UCharacterMovementComponent;
class AFromTheAshesRebornGameMode;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UMeleeAttackLogicComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	TObjectPtr<AFromTheAshesRebornGameMode> FTAGameMode;

	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	UDamageSystem* DamageSystemComponent;

	TArray<TObjectPtr<AActor>> AlreadyHitActors_L;
	TArray<TObjectPtr<AActor>> AlreadyHitActors_R;

	FOnAttackEnd OnAttackEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMeleeAttackRange MeleeAttackRange;

	bool ExtendAttackRange = false;

	bool IsActiveCollision = false;

	bool HasLeftWeapon = false;

public:

	float GetMeleeAttackRange(EMeleeAttackRange AttackRange);

	void EmptyHitActorsArray();

	UFUNCTION(BlueprintCallable, Category = "Melee Attack")
	void StartMeleeAttackCollisions();

	UFUNCTION(BlueprintCallable, Category = "Melee Attack")
	void EndMeleeAttackCollisions();
	
	bool MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits);

	UFUNCTION(BlueprintCallable, Category = "Melee Attack")
	void MeleeTraceCollisions();

	UFUNCTION(BlueprintCallable, Category = "Melee Attack")
	void MeleeAttackWarpToTarget(EMeleeAttackRange WarpRange, bool HasInput);

	void ResetMeleeAttackWarpToTarget();

	UMeleeAttackLogicComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
