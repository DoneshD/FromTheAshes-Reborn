#pragma once

#include "CoreMinimal.h"
#include "MaterialCueObject.h"
#include "FTAAbilitySystem/GameplayCues/FTAGameplayCueNotify_BurstLatent.h"
#include "OverlayCueNotify.generated.h"

class UOverlayCueObject;
class UNiagaraComponent;

UCLASS()
class FROMTHEASHESREBORN_API AOverlayCueNotify : public AFTAGameplayCueNotify_BurstLatent
{
	GENERATED_BODY()

protected:

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SpawnedNiagaraComponent;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> GroundStartNiagaraComponent;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> GroundEndNiagaraComponent;
	
protected:

	UPROPERTY()
	ACharacter* Character;

	float Alpha = 0.0;

	float Duration = 0.0;

	FTimerHandle TimerHandle;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> OverlayMaterialReference;

	bool IsActivated = false;

	FVector StartLocation;

	UPROPERTY()
	UOverlayCueObject* OverlayCueObject;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraSystem> NiagaraOverlay;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UseNiagaraOverlay = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraSystem> NiagaraGround;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UseNiagaraGround = true;
	

public:

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

	void InitializeParameters();
	void ActivateComponents();
	void StartOverlay();
	
};
