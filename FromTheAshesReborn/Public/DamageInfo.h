#pragma once

#include "CoreMinimal.h"
#include "EDamageResponse.h"
#include "EDamageType.h"
#include "DamageInfo.generated.h"

USTRUCT(BlueprintType)
struct FROMTHEASHESREBORN_API FDamageInfo
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Amount;

    EDamageType DamageType;

    EDamageResponse DamageResponse;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool ShouldDamageInvincible;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool CanBeBlocked;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CanBeParried;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool ShouldForceInterrupt;

};