#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "GroupCombatComponent.generated.h"

enum class EEnemyEngagementRole : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UGroupCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackTokensCount = 1;

	UPROPERTY(BlueprintReadWrite)
	int32 TokensUsedInCurrentAttack = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EEnemyEngagementRole EngagementRole = EEnemyEngagementRole::None;
	
protected:
	
	UGroupCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool ReserveAttackToken(int32 Amount);
	void ReturnAttackToken(int32 Amount);
	
};
