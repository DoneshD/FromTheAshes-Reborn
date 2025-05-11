﻿#pragma once

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

	// Layer to apply if the tag matches
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Layer;

	// Cosmetic tags required (all of these must be present to be considered a match)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FAnimLayerSelectionSet
{
	GENERATED_BODY()
		
	// List of layer rules to apply, first one that matches will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Layer))
	TArray<FAnimLayerMatchingTags> LayerRules;

	// The layer to use if none of the LayerRules matches
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> DefaultLayer;

	// Choose the best layer given the rules
	TSubclassOf<UAnimInstance> SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const;
};

//////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FAnimBodyStyleMatchingTags
{
	GENERATED_BODY()

	// Layer to apply if the tag matches
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	// Cosmetic tags required (all of these must be present to be considered a match)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FAnimBodyStyleSelectionSet
{
	GENERATED_BODY()
		
	// List of layer rules to apply, first one that matches will be used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Mesh))
	TArray<FAnimBodyStyleMatchingTags> MeshRules;

	// The layer to use if none of the LayerRules matches
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	// If set, ensures this physics asset is always used
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcedPhysicsAsset = nullptr;

	// Choose the best body style skeletal mesh given the rules
	USkeletalMesh* SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const;
};
