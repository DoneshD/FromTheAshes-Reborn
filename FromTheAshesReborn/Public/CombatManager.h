#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API ACombatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	TObjectPtr<AActor> AttackTarget;
	TArray<TObjectPtr<AActor>> Attackers;
	TArray<TObjectPtr<AActor>> WaitingAttackers;

	int MaxAttackersCount = 0;

public:	
	ACombatManager();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void HandleAttackRequest(TObjectPtr<AActor> Attacker);

};
