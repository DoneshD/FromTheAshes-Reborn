#pragma once

#include "CoreMinimal.h"
#include "EnumHelpers.h"
#include "Components/ActorComponent.h"
#include "MeleePropertiesComponent.generated.h"


class UNiagaraSystem;
class AFTACharacter;
class UEquipmentManagerComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UMeleePropertiesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TObjectPtr<AFTACharacter> FTAChar;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpatialDirection HitDirection = ESpatialDirection::None;

public:
	
	UMeleePropertiesComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void StartMeleeWeaponTrace(float TraceSize, ESpatialDirection Direction, UNiagaraSystem* Slash, UNiagaraSystem* Impact);

	UFUNCTION(BlueprintCallable)
	void EndWeaponTrace();
	
	UFUNCTION(BlueprintCallable)
	void SetMontagePlayRate(UAnimInstance* Mesh, UAnimMontage* Montage, float PlayRate);
};
