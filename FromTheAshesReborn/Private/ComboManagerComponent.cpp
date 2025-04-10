#include "ComboManagerComponent.h"
#include "DataAsset/FTAAbilityDataAsset.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
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

	// OnRegisterTestWindowTagEventDelegate.AddUniqueDynamic(this, &UComboManagerComponent::RegisterGameplayTagEvent);
	
}

void UComboManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UComboManagerComponent::InputQueueAllowedInputsBegin(TArray<TSubclassOf<UFTAGameplayAbility>> QueueableAbilityClasses)
{
	IsInInputQueueWindow = true;
	for (TSubclassOf<UFTAGameplayAbility> AbilityClass : QueueableAbilityClasses)
	{
		if (AbilityClass)
		{
			if (AbilityClass.GetDefaultObject())
			{
				// OnRegisterInputWindowTagEventDelegate.Broadcast(AbilityClass.GetDefaultObject()->ComboWindowTag);
			}
		}
	}
}

void UComboManagerComponent::InputQueueUpdateAllowedInputsEnd()
{
	IsInInputQueueWindow = false;
	FTAASC->QueuedAbilitySpec = nullptr;
}


void UComboManagerComponent::RegisterGameplayTagEvent(FGameplayTag ComboWindowTag)
{
	RemoveGameplayTagEvent(ComboWindowTag);
	
	FDelegateHandle Handle = FTAASC->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UComboManagerComponent::ComboWindowTagChanged);
	TagDelegateHandles.Add(ComboWindowTag, Handle);
}

void UComboManagerComponent::ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0 && FTAASC->HasMatchingGameplayTag(CallbackTag))
	{
		FTimerHandle& ComboWindowTimer = TagTimerHandles.FindOrAdd(CallbackTag);
		GetWorld()->GetTimerManager().SetTimer(ComboWindowTimer, [this, CallbackTag]()
		{
			ComboWindowOpen(CallbackTag);
		}, 0.01f, true);
	}
	else
	{
		RemoveGameplayTagEvent(CallbackTag);
	}
}

void UComboManagerComponent::ComboWindowOpen(FGameplayTag ComboWindowTag)
{
	//TODO: Casting in tick!!!!!
	UFTAGameplayAbility* QueuedAbility = Cast<UFTAGameplayAbility>(FTAASC->QueuedAbilitySpec.Ability);
	if(!QueuedAbility)
	{
		// UE_LOG(LogTemp, Error, TEXT("QueuedAbility NULL"));
		return;
	}
	
	if(ComboWindowTag.MatchesTag(QueuedAbility->QueueWindowTag))
	{
		
		ProceedToNextAbility(QueuedAbility->GetClass());
	}
}

void UComboManagerComponent::ProceedToNextAbility(TSubclassOf<UGameplayAbility> AbilityToActivateClass)
{
	//TODO: Need to change this 
	FTAASC->CancelAllAbilities();
	
	FTAASC->QueuedAbilitySpec = nullptr;
	
	bool bActivateAbility = FTAASC->TryActivateAbilityByClass(AbilityToActivateClass);
	
	if(!bActivateAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceedToNextAbility: Ability did not activate"));
	}
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

void UComboManagerComponent::RemoveGameplayTagEvent(FGameplayTag ComboWindowTag)
{
	if (TagTimerHandles.Contains(ComboWindowTag))
	{
		FTimerHandle& ComboWindowTimer = TagTimerHandles[ComboWindowTag];
		if (GetWorld()->GetTimerManager().IsTimerActive(ComboWindowTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(ComboWindowTimer);
		}
		TagTimerHandles.Remove(ComboWindowTag);
	}
	
	if (TagDelegateHandles.Contains(ComboWindowTag))
	{
		FDelegateHandle Handle = TagDelegateHandles[ComboWindowTag];
		FTAASC->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).Remove(Handle);
		TagDelegateHandles.Remove(ComboWindowTag);
	}
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

