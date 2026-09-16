#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "FTAAbilitySystem/FTAAbilitySourceInterface.h"
#include "FTAGameplayEffectContext.generated.h"

class UFTACueObject;
class UNiagaraSystem;
class AActor;
class FArchive;
class IFTAAbilitySourceInterface;
class UObject;
class UPhysicalMaterial;

USTRUCT()
struct FFTAGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UFTACueObject> CueObject;
	
	FFTAGameplayEffectContext() : FGameplayEffectContext(){}
	FFTAGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser) : FGameplayEffectContext(InInstigator, InEffectCauser){}

	virtual FGameplayEffectContext* Duplicate() const override;
	
	static FROMTHEASHESREBORN_API FFTAGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);
	void SetAbilitySource(const UObject* InObject, float InSourceLevel);

	const IFTAAbilitySourceInterface* GetAbilitySource() const;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FFTAGameplayEffectContext::StaticStruct();
	}

protected:
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilitySourceObject;
};
