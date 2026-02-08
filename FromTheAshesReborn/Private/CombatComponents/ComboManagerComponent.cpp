#include "CombatComponents/ComboManagerComponent.h"

#include "DataAsset/AttackAbilityDataAsset.h"
#include "DataAsset/FTAAbilityDataAsset.h"
#include "DataAsset/HitReactionDataAsset.h"
#include "DataAsset/MeleeAbilityDataAsset.h"
#include "EventObjects/HitEventObject.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_Attack.h"
#include "FTACustomBase/FTACharacter.h"


UComboManagerComponent::UComboManagerComponent(): FTAASC(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UComboManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	const AFTACharacter* FTAChar = Cast<AFTACharacter>(GetOwner());
	if(!FTAChar)
	{
		UE_LOG(LogTemp, Error, TEXT("UComboManagerComponent::BeginPlay - Owner is not FTAChar"));
		return;
	}
	
	FTAASC = FTAChar->GetFTAAbilitySystemComponent();
	
	if(!FTAASC)
	{
		UE_LOG(LogTemp, Error, TEXT("UComboManagerComponent::BeginPlay - No ASComponent found"));
		return;
	}

}

void UComboManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UComboManagerComponent::PrintGameplayTagsInContainer(const FGameplayTagContainer& Container)
{
	if (Container.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("PrintGameplayTagsInContainer - Container is empty."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("========== GameplayTagContainer Contents =========="));

	for (const FGameplayTag& Tag : Container)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Tag.ToString());
	}

	UE_LOG(LogTemp, Log, TEXT("=========================================="));
}

FGameplayTagContainer& UComboManagerComponent::GetCurrentComboContainer()
{
	return CurrentComboTagContainer;
}

int UComboManagerComponent::GetCurrentComboIndex() const
{
	return CurrentComboIndex;
}

void UComboManagerComponent::SetCurrentComboIndex(int Index)
{
	CurrentComboIndex = Index;
}


TObjectPtr<UFTAAbilityDataAsset> UComboManagerComponent::GetMeleeAssetByRequirements(
	TArray<UFTAAbilityDataAsset*> AbilityAssets)
{
	for (UFTAAbilityDataAsset* Asset : AbilityAssets)
	{
		if (!Asset) continue;

		if (Asset->RequiredIndex == GetCurrentComboIndex())
		{
			if(Asset->CheckForRequirementTags)
			{
				if (GetCurrentComboContainer().HasAll(Asset->RequiredComboTags))
				{
					// if (PauseCurrentAttack)
					if (false)
					{
						if (Asset->RequiredPause)
						{
							return Asset;
						}
					}
					else
					{
						if (!Asset->RequiredPause)
						{
							return Asset;
						}
					}
				}
			}
			else
			{
				return Asset;
			}
		}
	}
	
	GetCurrentComboContainer().Reset();
	SetCurrentComboIndex(0);
	return AbilityAssets[0];
}

TObjectPtr<UHitReactionDataAsset> UComboManagerComponent::GetHitAssetByRequirements(TArray<UHitReactionDataAsset*> InHitAssets, const UHitEventObject* InHitObject)
{
	TArray<UHitReactionDataAsset*> AssetsToTry;
	
	// UE_LOG(LogTemp, Warning, TEXT("%s"),
	// *StaticEnum<ESpatialDirection>()->GetNameStringByValue((int64)InHitObject->HitData.HitDirection));
	
	for (UHitReactionDataAsset* Asset : InHitAssets)
	{
		if(Asset->HitData.Direction == InHitObject->HitData.HitDirection || Asset->HitData.Direction == ESpatialDirection::Any)
		{
			if(Asset->MontageToPlay)
			{
				AssetsToTry.Add(Asset);
			}
		}
	}
	if(AssetsToTry.Num() > 0)
	{
		int Selection = FMath::RandRange(0, AssetsToTry.Num() - 1);
		return AssetsToTry[Selection];
	}
	return nullptr;
}

TObjectPtr<UFTAAbilityDataAsset> UComboManagerComponent::GetRangedAssetByRequirements(TArray<UFTAAbilityDataAsset*> AbilityAssets)
{

	//Temporary for now
	for (UFTAAbilityDataAsset* Asset : AbilityAssets)
	{
		if (!Asset) continue;

		if (GetCurrentComboContainer().HasAll(Asset->RequiredComboTags))
		{
			if (Asset->RequiredIndex == GetCurrentComboIndex())
			{
				// if (PauseCurrentAttack)
				if (false)
				{
					if (Asset->RequiredPause)
					{
						return Asset;
					}
				}
				else
				{
					if (!Asset->RequiredPause)
					{
						return Asset;
					}
				}
			}
		}
	}
	
	GetCurrentComboContainer().Reset();
	SetCurrentComboIndex(0);
	return AbilityAssets[0];
}
