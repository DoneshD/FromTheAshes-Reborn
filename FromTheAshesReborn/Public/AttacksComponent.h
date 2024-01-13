
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageSystem/DamageSystem.h"
#include "AttacksComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);

class UPlayMontageCallbackProxy;

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

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAttackEnd OnAttackEnd;

	UFUNCTION(BlueprintCallable, Category = "Attack Collision")
	void StartAttackCollisions();

	UFUNCTION(BlueprintCallable, Category = "Attack Collision")
	void EndAttackCollisions();

	void EmptyHitActorsArray();

	void LightMeleeAttack(TObjectPtr<UAnimMontage> LightMeleeAttack);

	void MeleeTraceCollisions();

	void HandleMontageCompleted(UAnimMontage* Montage, bool bInterrupted);

	void HandleMontageInterrupted(UAnimMontage* Montage, bool bInterrupted);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
