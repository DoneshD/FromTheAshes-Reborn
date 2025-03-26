#pragma once
	 
#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FTAAT_OnTick.generated.h"
	 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);
	 

UCLASS()
class FROMTHEASHESREBORN_API UFTAAT_OnTick : public UAbilityTask
{
	GENERATED_BODY()
	 
	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;
	 
public:
	 
	UFTAAT_OnTick(const FObjectInitializer& ObjectInitializer);
		
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
		
};