#pragma once

#include "CoreMinimal.h"
#include "Enums/EDamageResponse.h"
#include "Enums/EDamageType.h"
#include "Enums/EFacingDirection.h"
#include "DamageInfo.generated.h"

USTRUCT(BlueprintType)
struct FROMTHEASHESREBORN_API FDamageInfo
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float DamageAmount;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    EDamageType DamageType;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    EDamageResponse DamageResponse;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool ShouldDamageInvincible;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool CanBeBlocked;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CanBeParried;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool ShouldForceInterrupt;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    EFacingDirection FacingDirection;
};