#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "HitReactionComponent.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FHitReactionDataStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	TSubclassOf<UGameplayEffect> HitEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	FGameplayTag HitTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	FGameplayTag CharacterOrientationTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	FGameplayTagContainer CharacterStateTags;
	
	FHitReactionDataStruct()
	:
	HitEffect(nullptr),
	HitTag(FGameplayTag::EmptyTag),
	CharacterOrientationTag(FGameplayTag::EmptyTag),
	CharacterStateTags(FGameplayTag::EmptyTag)
	{}
	
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UHitReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Hit")
	TArray<FHitReactionDataStruct> HitReactions;


protected:
	UHitReactionComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Hit")
	FHitReactionDataStruct GetAssociatedHitReaction(FGameplayTag Index);
	
};
