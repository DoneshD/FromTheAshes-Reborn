#include "ComboManagerComponent.h"
#include "DataAsset/FTAAbilityDataAsset.h"
#include "DataAsset/MeleeAbilityDataAsset.h"
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

bool UComboManagerComponent::FindMatchingAssetToTagContainer(const TArray<UMeleeAbilityDataAsset*>& AbilityDataAssets, TObjectPtr<UMeleeAbilityDataAsset>& OutMatchingAbilityDataAsset)
{
	for (int32 Index = 0; Index < AbilityDataAssets.Num(); ++Index)
	{
		if (AbilityDataAssets[Index])
		{
			if (GetCurrentComboContainer().HasAll(AbilityDataAssets[Index]->RequiredTags))
			{
				if(AbilityDataAssets[Index]->RequiredIndex == GetCurrentComboIndex())
				{
					if(PauseCurrentAttack)
					{
						if(AbilityDataAssets[Index]->RequiredPause)
						{
							OutMatchingAbilityDataAsset = AbilityDataAssets[Index];
							return true;
						}
					}
					else
					{
						if(!AbilityDataAssets[Index]->RequiredPause)
						{
							OutMatchingAbilityDataAsset = AbilityDataAssets[Index];
							return true;
						}
					}
				}
			}
		}
	}
	GetCurrentComboContainer().Reset();
	SetCurrentComboIndex(0);
	OutMatchingAbilityDataAsset = AbilityDataAssets[0];
	return true;	
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

