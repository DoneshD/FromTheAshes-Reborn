#pragma once

#include "CoreMinimal.h"
#include "FTAAbilitySystem/GameplayCues/FTACueObject.h"
#include "NiagaraCueObject.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FNiagaraStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector Location;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector LocationOffset;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator Rotation;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FRotator RotationOffset;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FName BoneSocketName;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool AttachToOwner = false;
	
};

UCLASS()
class FROMTHEASHESREBORN_API UNiagaraCueObject : public UFTACueObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FNiagaraStruct> NiagaraCueArray;
	
	
};
