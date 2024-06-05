#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackTokenSystemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UAttackTokenSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackTokensCount = 1;

	int TokensUsedInCurrentAttack;

	TMap<AActor*, int> ReservedAttackTokensMap;


public:	
	UAttackTokenSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	bool AttackStart(AActor* AttackTarget, int TokensNeeded);

	UFUNCTION(BlueprintCallable)
	void AttackEnd(AActor* AttackTarget);

	UFUNCTION(BlueprintCallable)
	bool ReserveAttackToken(int Amount);

	UFUNCTION(BlueprintCallable)
	void ReturnAttackToken(int Amount);

	UFUNCTION()
	void StoreAttackTokens(AActor* AttackTarget, int Amount);
		
};
