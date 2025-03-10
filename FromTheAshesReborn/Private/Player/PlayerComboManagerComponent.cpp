﻿#include "Player/PlayerComboManagerComponent.h"
#include "AbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

#include "Player/FTAPlayerController.h"

UPlayerComboManagerComponent::UPlayerComboManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerComboManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->GetInstigatorController())
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerComboManagerComponent: No controller found"));
		return;
	}
	
	PC = Cast<AFTAPlayerController>(GetOwner()->GetInstigatorController());
	
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerComboManagerComponent: No player controller found"));
		return;
	}

	if (!PC->GetFTAAbilitySystemComponent())
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerComboManagerComponent: No ASC in player controller found"));
		return;
	}
	

	ASComponent = PC->GetFTAAbilitySystemComponent();
	if(!ASComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UPlayerComboManagerComponent: No ASComponent found"));
		return;
	}

	OnRegisterWindowTagEventDelegate.AddUniqueDynamic(this, &UPlayerComboManagerComponent::RegisterGameplayTagEvent);
	
}

void UPlayerComboManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerComboManagerComponent::PrintCurrentComboContainer()
{
	for (const FGameplayTag& Tag : CurrentComboTagContainer)
	{
		if (*Tag.ToString())
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentComboTagContainer Tag: %s"), *Tag.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CurrentComboTagContainer Tag is NULL"));
		}
	}
}

void UPlayerComboManagerComponent::InputQueueAllowedInputsBegin(
	TArray<TSubclassOf<UFTAGameplayAbility>> QueueableAbilityClasses)
{
	IsInInputQueueWindow = true;
	for (TSubclassOf<UFTAGameplayAbility> AbilityClass : QueueableAbilityClasses)
	{
		if (AbilityClass)
		{
			if (AbilityClass.GetDefaultObject())
			{
				OnRegisterWindowTagEventDelegate.Broadcast(AbilityClass.GetDefaultObject()->ComboWindowTag);
			}
		}
	}
}

void UPlayerComboManagerComponent::InputQueueUpdateAllowedInputsEnd()
{
	IsInInputQueueWindow = false;
	ASComponent->QueuedAbilitySpec = nullptr;
}

FGameplayTagContainer& UPlayerComboManagerComponent::GetCurrentComboContainer()
{
	return CurrentComboTagContainer;
}

int UPlayerComboManagerComponent::GetCurrentComboIndex()
{
	return CurrentComboIndex;
}

void UPlayerComboManagerComponent::SetCurrentComboIndex(int Index)
{
	CurrentComboIndex = Index;
}

void UPlayerComboManagerComponent::ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{

	if (NewCount > 0 && ASComponent->HasMatchingGameplayTag(CallbackTag))
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


void UPlayerComboManagerComponent::ComboWindowOpen(FGameplayTag ComboWindowTag)
{
	//Casting in tick!!!!!
	UFTAGameplayAbility* QueuedAbility = Cast<UFTAGameplayAbility>(ASComponent->QueuedAbilitySpec.Ability);
	if(!QueuedAbility)
	{
		// UE_LOG(LogTemp, Error, TEXT("QueuedAbility NULL"));
		return;
	}
	
	if(ComboWindowTag.MatchesTag(QueuedAbility->ComboWindowTag))
	{
		
		ProceedToNextAbility(QueuedAbility->GetClass());
	}
}

void UPlayerComboManagerComponent::ProceedToNextAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	ASComponent->CancelAllAbilities();
	ASComponent->QueuedAbilitySpec = nullptr;
	
	bool bActivateAbility = ASComponent->TryActivateAbilityByClass(AbilityClass);
	
	if(!bActivateAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceedToNextAbility: Ability did not activate"));
	}
}

void UPlayerComboManagerComponent::RegisterGameplayTagEvent(FGameplayTag ComboWindowTag)
{
	RemoveGameplayTagEvent(ComboWindowTag);
	
	FDelegateHandle Handle = ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerComboManagerComponent::ComboWindowTagChanged);
	TagDelegateHandles.Add(ComboWindowTag, Handle);
}


void UPlayerComboManagerComponent::RemoveGameplayTagEvent(FGameplayTag ComboWindowTag)
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
        ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).Remove(Handle);
        TagDelegateHandles.Remove(ComboWindowTag);
    }
}
