
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageSystem/DamageSystem.h"
#include "AttacksComponent.generated.h"

DECLARE_DELEGATE(FOnAttackEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UAttacksComponent : public UActorComponent
{
	GENERATED_BODY()

protected:	

	UAttacksComponent();

	UDamageSystem* DamageSystemComponent;

	TArray<TObjectPtr<AActor>> AlreadyHitActors_L;
	TArray<TObjectPtr<AActor>> AlreadyHitActors_R;

	bool MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits);

private:

	bool bActiveCollision = false;

public:	

	FOnAttackEnd OnAttackEnd;

	void EmptyHitActorsArray();

	UFUNCTION(BlueprintCallable, Category = "Attack Collision")
	void StartAttackCollisions();

	UFUNCTION(BlueprintCallable, Category = "Attack Collision")
	void EndAttackCollisions();

	UFUNCTION(BlueprintCallable, Category = "Attack Collision")
	void FinishLightMeleeAttack();

	void LightMeleeAttack(TObjectPtr<UAnimMontage> LightMeleeAttack);

	void MeleeTraceCollisions();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
