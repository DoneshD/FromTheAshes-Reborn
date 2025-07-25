﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "WeaponActorBase.generated.h"

enum class EHitReactionDirection : uint8;
class UTracingComponent;
class UDidItHitActorComponent;

UCLASS()
class FROMTHEASHESREBORN_API AWeaponActorBase : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tracing")
	TObjectPtr<UTracingComponent> TracingComponent;

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

	//TODO: Change later
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Direction")
	EHitReactionDirection HitDirection;

public:
	
	AWeaponActorBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartWeaponTracing();

	UFUNCTION(BlueprintCallable)
	void EndWeaponTracing();
	
};
