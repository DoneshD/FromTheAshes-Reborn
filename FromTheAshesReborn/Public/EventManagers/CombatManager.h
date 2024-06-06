#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API ACombatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	
	int MaxAttackersCount = 0;

	TObjectPtr<AActor> AttackTarget;
	TArray<TObjectPtr<AActor>> Attackers;
	TArray<TObjectPtr<AActor>> Oribters;

public:	

	ACombatManager();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void HandleAttackRequest(AActor* Attacker);

	void HandleAttackerSwapRequest(AActor* Attacker);

	void EngageRandomOrbiter();

	void DisengageRandomAttacker();

	void HandleDeath(TObjectPtr<AActor> ActorRef);


};
