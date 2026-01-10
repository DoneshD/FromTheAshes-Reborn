#include "CombatComponents/ComboManagerComponent.h"

#include "DataAsset/AttackAbilityDataAsset.h"
#include "DataAsset/FTAAbilityDataAsset.h"
#include "DataAsset/MeleeAbilityDataAsset.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_Attack.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack.h"
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

bool UComboManagerComponent::FindMatchingMeleeAssetToTagContainer(const FAttackComboType& AttackComboTypes,
	TObjectPtr<UAttackAbilityDataAsset>& OutMatchingAttackAsset)
{
	TArray<TObjectPtr<UAttackAbilityDataAsset>> CombinedAssets;
	CombinedAssets.Append(AttackComboTypes.NormalAttacks);
	CombinedAssets.Append(AttackComboTypes.PauseAttacks);
	CombinedAssets.Append(AttackComboTypes.VariantAttacks);

	for (UAttackAbilityDataAsset* Asset : CombinedAssets)
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
						OutMatchingAttackAsset = Asset;
						return true;
					}
				}
				else
				{
					if (!Asset->RequiredPause)
					{
						OutMatchingAttackAsset = Asset;
						return true;
					}
				}
			}
		}
	}
	
	GetCurrentComboContainer().Reset();
	SetCurrentComboIndex(0);
	OutMatchingAttackAsset = AttackComboTypes.NormalAttacks[0];
	return true;
}

TObjectPtr<UFTAAbilityDataAsset> UComboManagerComponent::GetAbilityAssetByRequirements(
	TArray<UFTAAbilityDataAsset*> AbilityAssets)
{
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