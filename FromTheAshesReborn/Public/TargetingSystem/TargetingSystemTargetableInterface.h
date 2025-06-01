#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetingSystemTargetableInterface.generated.h"

UINTERFACE(Blueprintable)
class UTargetingSystemTargetableInterface : public UInterface
{
	GENERATED_BODY()
};


class FROMTHEASHESREBORN_API ITargetingSystemTargetableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Target System")
	bool IsTargetable() const;
};
