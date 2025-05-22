#pragma once
	 
#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FTAAT_OnTick.generated.h"
	 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);
	 

UCLASS()
class FROMTHEASHESREBORN_API UFTAAT_OnTick : public UAbilityTask
{
	GENERATED_BODY()
	 
protected:

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	UFTAAT_OnTick(const FObjectInitializer& ObjectInitializer);
	
public:
	
	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;
	

	UFUNCTION(BlueprintCallable, Category = "FTAAbilityTasks | Tasks", meta = (DisplayName = "Start OnTick Task"))
	static UFTAAT_OnTick* StartTicking(UGameplayAbility* OwningAbility);

	virtual FString GetDebugString() const override;

protected:
	virtual void OnDestroy(bool AbilityEnded) override;
		
};