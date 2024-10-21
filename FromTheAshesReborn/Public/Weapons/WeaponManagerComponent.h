#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/WeaponInterface.h"
#include "WeaponManagerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UWeaponManagerComponent : public UActorComponent, public IWeaponInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AFTAWeapon> LightWeaponClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AFTAWeapon> HeavyWeaponClass;

	TObjectPtr<AFTAWeapon> LightWeapon;

	TObjectPtr<AFTAWeapon> HeavyWeapon;

	UFUNCTION(BlueprintCallable)
	virtual AFTAWeapon* GetLightWeapon() override;

	UFUNCTION(BlueprintCallable)
	virtual AFTAWeapon* GetHeavyWeapon() override;

public:
	UWeaponManagerComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
};
