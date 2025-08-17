#pragma once

#include "CoreMinimal.h"
#include "EnumHelpers.h"
#include "Components/ActorComponent.h"
#include "MeleePropertiesComponent.generated.h"


class AFTACharacter;
class UEquipmentManagerComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UMeleePropertiesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpatialDirection HitDirection;

	UPROPERTY()
	TObjectPtr<AFTACharacter> FTAChar;


public:
	
	UMeleePropertiesComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void StartWeaponTrace();

	UFUNCTION(BlueprintCallable)
	void EndWeaponTrace();
	
	UFUNCTION(BlueprintCallable)
	void SetMontagePlayRate(UAnimInstance* Mesh, UAnimMontage* Montage, float PlayRate);
};
