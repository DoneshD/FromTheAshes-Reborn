﻿#include "ComboManagerComponent.h"
#include "DataAsset/FTAAbilityDataAsset.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"


UComboManagerComponent::UComboManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UComboManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	AFTACharacter* FTAChar = Cast<AFTACharacter>(GetOwner());
	if(!FTAChar)
	{
		UE_LOG(LogTemp, Error, TEXT("UComboManagerComponent: Owner is not FTAChar"));
		return;
	}
	
	FTAASC = FTAChar->GetFTAAbilitySystemComponent();
	
	if(!FTAASC)
	{
		UE_LOG(LogTemp, Error, TEXT("UComboManagerComponent: No ASComponent found"));
		return;
	}

}

void UComboManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UComboManagerComponent::FindMatchingAssetToTagContainer(const TArray<UFTAAbilityDataAsset*>& AbilityDataAssets, TObjectPtr<UFTAAbilityDataAsset>& OutMatchingAbilityDataAsset)
{
	for (int32 Index = 0; Index < AbilityDataAssets.Num(); ++Index)
	{
		if (AbilityDataAssets[Index])
		{
			if (GetCurrentComboContainer().HasAll(AbilityDataAssets[Index]->RequiredTags))
			{
				if(AbilityDataAssets[Index]->RequiredIndex == GetCurrentComboIndex())
				{
					OutMatchingAbilityDataAsset = AbilityDataAssets[Index];
					return true;
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

int UComboManagerComponent::GetCurrentComboIndex()
{
	return CurrentComboIndex;
}

void UComboManagerComponent::SetCurrentComboIndex(int Index)
{
	CurrentComboIndex = Index;
}

