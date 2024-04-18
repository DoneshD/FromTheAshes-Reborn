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

public:	
	ACombatManager();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
