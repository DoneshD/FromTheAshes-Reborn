#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "FTAGameplayEffectContext.generated.h"

class AActor;
class FArchive;
class ILyraAbilitySourceInterface;
class UObject;
class UPhysicalMaterial;

USTRUCT()
struct FFTAGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
	FFTAGameplayEffectContext() : FGameplayEffectContext(){}
	FFTAGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser) : FGameplayEffectContext(InInstigator, InEffectCauser){}

	static FROMTHEASHESREBORN_API FFTAGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);
	
};

// class FROMTHEASHESREBORN_API FTAGameplayEffectContext
// {
// public:
// 	FTAGameplayEffectContext();
// 	~FTAGameplayEffectContext();
// };
