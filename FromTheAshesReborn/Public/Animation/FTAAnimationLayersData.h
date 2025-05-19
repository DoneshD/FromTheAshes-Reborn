#pragma once

#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"
#include "FTAAnimationLayersData.generated.h"

class UAnimInstance;
class UPhysicsAsset;
class USkeletalMesh;

USTRUCT(BlueprintType)
struct FAnimLayerMatchingTags
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Layer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FFTAAnimLayerSelectionSet
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Layer))
	TArray<FAnimLayerMatchingTags> LayerRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> DefaultLayer;

	TSubclassOf<UAnimInstance> SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const;
};


USTRUCT(BlueprintType)
struct FAnimBodyStyleMatchingTags
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FAnimBodyStyleSelectionSet
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Mesh))
	TArray<FAnimBodyStyleMatchingTags> MeshRules;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcedPhysicsAsset = nullptr;

	USkeletalMesh* SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const;
};
