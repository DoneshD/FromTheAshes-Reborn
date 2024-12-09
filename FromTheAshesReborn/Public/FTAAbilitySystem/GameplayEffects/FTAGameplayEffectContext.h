#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "SWarningOrErrorBox.h"
#include "FTAAbilitySystem/FTAAbilitySourceInterface.h"
#include "FTAGameplayEffectContext.generated.h"

class AActor;
class FArchive;
class IFTAAbilitySourceInterface;
class UObject;
class UPhysicalMaterial;

USTRUCT()
struct FFTAGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
	FFTAGameplayEffectContext() : FGameplayEffectContext(){}
	FFTAGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser) : FGameplayEffectContext(InInstigator, InEffectCauser){}

	static FROMTHEASHESREBORN_API FFTAGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);
	void SetAbilitySource(const IFTAAbilitySourceInterface* InObject, float InSourceLevel);

	const IFTAAbilitySourceInterface* GetAbilitySource() const;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		UE_LOG(LogTemp, Error, TEXT("???"));
		return FFTAGameplayEffectContext::StaticStruct();
	}

protected:
	/** Ability Source object (should implement ILyraAbilitySourceInterface). NOT replicated currently */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};

// class FROMTHEASHESREBORN_API FTAGameplayEffectContext
// {
// public:
// 	FTAGameplayEffectContext();
// 	~FTAGameplayEffectContext();
// };
