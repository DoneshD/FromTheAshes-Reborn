#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterOverlayComponent.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UCharacterOverlayComponent : public UActorComponent
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

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TimerSpeed = 3.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UMaterialInterface> OverlayMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraSystem> NiagaraOverlay;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UseNiagaraOverlay = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraSystem> NiagaraGround;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UseNiagaraGround = true;

protected:
	UCharacterOverlayComponent();
	virtual void BeginPlay() override;

	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ToggleOverlay();

public:
	UFUNCTION(BlueprintCallable)
	void StartEffect();

	
	
};
