#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GroupCombatComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UGroupCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackTokensCount = 1;

	int TokensUsedInCurrentAttack;
	
protected:
	
	UGroupCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool ReserveAttackToken(int32 Amount);
	void ReturnAttackToken(int32 Amount);
	
};
