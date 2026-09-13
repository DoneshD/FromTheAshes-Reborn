#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ActorComponent.h"
#include "DismembermentComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UDismembermentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FName HitBoneName;

protected:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;


protected:
	
	UDismembermentComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CaptureDismembermentData(FName InBoneName);

	void RenameBoneName();

	UFUNCTION(BlueprintCallable)
	void ApplyDismemberment(FVector InImpulse, FVector InHitLocation, FName InBoneName, UNiagaraSystem* InNiagaraSystem);

};
