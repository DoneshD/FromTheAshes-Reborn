#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotifyTypes.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "SlashCueObject.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FSlashCueInfoStruct
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (Categories = "GameplayCue"))
	FGameplayTag SlashCueTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNiagaraSystem> SlashNiagaraEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TObjectPtr<USoundBase>> SlashSoundsArray;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MinPitch = 0.90f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxPitch = 1.50f;
};

UCLASS(BlueprintType, Blueprintable)
class FROMTHEASHESREBORN_API USlashCueObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlashCueInfoStruct SlashCueInfo;
	
};
