#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FTAAbilityDataObject.generated.h"

USTRUCT(BlueprintType)
struct FBaseAbilityDataStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BaseFloatTest;
	
};

USTRUCT(BlueprintType)
struct FAttackAbilityDataStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBaseAbilityDataStruct BaseData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChildFloatTest;
	
};

USTRUCT(BlueprintType)
struct FMeleeAbilityDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackAbilityDataStruct AttackData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GrandChildFloatTest;
};


UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class FROMTHEASHESREBORN_API UFTAAbilityDataObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FBaseAbilityDataStruct AbilityDataTest;
	
};