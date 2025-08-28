#pragma once

#include "CoreMinimal.h"
#include "EnumHelpers.h"
#include "Components/ActorComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack.h"
#include "MeleePropertiesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMeleeRuntimeDataSetSignature, FMeleeRuntimeDataStruct, InMeleeData);

class UNiagaraSystem;
class AFTACharacter;
class UEquipmentManagerComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UMeleePropertiesComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<AFTACharacter> FTAChar;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	TSubclassOf<UGameplayEffect> StaggerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	TSubclassOf<UGameplayEffect> SuspendEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	TSubclassOf<UGameplayEffect> LaunchEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	TSubclassOf<UGameplayEffect> KnockdownEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	TSubclassOf<UGameplayEffect> KnockbackEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	TSubclassOf<UGameplayEffect> SlamEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	TSubclassOf<UGameplayEffect> DownFlinchEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit Effects")
	TSubclassOf<UGameplayEffect> BounceEffect;

public:

	UPROPERTY(BlueprintCallable, Category = "Events")
	FOnMeleeRuntimeDataSetSignature OnSetMeleeData;
	
	UMeleePropertiesComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
