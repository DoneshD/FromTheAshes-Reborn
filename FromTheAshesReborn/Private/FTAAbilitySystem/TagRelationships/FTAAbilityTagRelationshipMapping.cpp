﻿#include "FTAAbilitySystem/TagRelationships/FTAAbilityTagRelationshipMapping.h"

void UFTAAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FFTAAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			if (OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Tags.AbilityTagsToBlock);
			}
			if (OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Tags.AbilityTagsToCancel);
			}
		}
	}
}

void UFTAAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FFTAAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			if (OutActivationRequired)
			{
				OutActivationRequired->AppendTags(Tags.ActivationRequiredTags);
			}
			if (OutActivationBlocked)
			{
				for (const FGameplayTag& Tag : Tags.ActivationBlockedTags)
				{
					UE_LOG(LogTemp, Warning, TEXT("INSIDE ActivationBlockedTags - %s"), *Tag.ToString());
				}
				OutActivationBlocked->AppendTags(Tags.ActivationBlockedTags);
			}
		}
	}
}

bool UFTAAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags,
	const FGameplayTag& ActionTag) const
{
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FFTAAbilityTagRelationship& Tags = AbilityTagRelationships[i];

		if (Tags.AbilityTag == ActionTag && Tags.AbilityTagsToCancel.HasAny(AbilityTags))
		{
			return true;
		}
	}
	return false;
}
