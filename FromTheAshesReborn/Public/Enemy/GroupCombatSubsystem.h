#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GroupCombatSubsystem.generated.h"

UCLASS()
class FROMTHEASHESREBORN_API UGroupCombatSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	int32 AttackTokensCount;

protected:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
};
