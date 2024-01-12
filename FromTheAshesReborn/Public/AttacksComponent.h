
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageSystem/DamageSystem.h"
#include "AttacksComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UAttacksComponent : public UActorComponent
{
	GENERATED_BODY()

protected:	

	UAttacksComponent();

	UDamageSystem* DamageSystemComponent;

	TArray<TObjectPtr<AActor>> AlreadyHitActors_L;
	TArray<TObjectPtr<AActor>> AlreadyHitActors_R;

	bool MeleeWeapomSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits);

private:

	bool bActiveCollision = false;

public:	

	UFUNCTION(BlueprintCallable, Category = "Attack Collision")
	void StartAttackCollisions();

	UFUNCTION(BlueprintCallable, Category = "Attack Collision")
	void EndAttackCollisions();

	void EmptyHitActorsArray();

	void LightMeleeAttack();

	void MeleeTraceCollisions();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
