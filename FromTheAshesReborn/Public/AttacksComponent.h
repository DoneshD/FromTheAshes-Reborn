
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/DamagableInterface.h"
#include "AttacksComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UAttacksComponent : public UActorComponent, public IDamagableInterface
{
	GENERATED_BODY()

protected:	

	UAttacksComponent();


	TArray<TObjectPtr<AActor>> AlreadyHitActors_L;
	TArray<TObjectPtr<AActor>> AlreadyHitActors_R;

	bool MeleeWeapomSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits);

	void MeleeTraceCollisions();

private:

public:	

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Damagable Interface functions

	UFUNCTION()
	virtual float NativeGetCurrentHealth() override;

	UFUNCTION()
	virtual float NativeGetMaxHealth() override;

	UFUNCTION()
	virtual bool NativeIsDead() override;

	UFUNCTION()
	virtual float NativeHeal(float NewHeatlh) override;

	UFUNCTION()
	virtual bool NativeTakeDamage(FDamageInfo DamageInfo) override;
};
