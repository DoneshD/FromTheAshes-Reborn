#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "WeaponActorBase.generated.h"

class UDidItHitActorComponent;

UCLASS()
class FROMTHEASHESREBORN_API AWeaponActorBase : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tracing")
	TObjectPtr<UDidItHitActorComponent> DidItHitActorComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<UNiagaraComponent> NS_SwordTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<UNiagaraSystem> SwordTrail;

	UPROPERTY()
	UNiagaraComponent* TrailComponent;

	FVector SpawnTrailLocation;
	FRotator SpawnTrailRotation;

	float TrailLength;

public:
	
	AWeaponActorBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartWeaponTracing();

	UFUNCTION(BlueprintCallable)
	void EndWeaponTracing();

	UFUNCTION(BlueprintCallable)
	void StartWeaponTrail(UNiagaraSystem* TestSystem);

	UFUNCTION(BlueprintCallable)
	void EndWeaponTrail();
	
};
