#pragma once
	 
#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FTAAT_OnTick.generated.h"
	 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);
	 

UCLASS()
class FROMTHEASHESREBORN_API UFTAAT_OnTick : public UAbilityTask
{
	GENERATED_BODY()
	 

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;

	UFTAAT_OnTick(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	// This is the static function you use in your ability
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "Start OnTick Task", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UFTAAT_OnTick* StartTicking(UGameplayAbility* OwningAbility);

protected:
	virtual void OnDestroy(bool AbilityEnded) override;
		
};