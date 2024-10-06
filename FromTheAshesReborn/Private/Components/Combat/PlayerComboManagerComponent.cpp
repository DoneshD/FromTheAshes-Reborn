#include "Components/Combat/PlayerComboManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "Player/FTAPlayerController.h"

UPlayerComboManagerComponent::UPlayerComboManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UPlayerComboManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	if(!GetOwner()->GetInstigatorController())
	{
		UE_LOG(LogTemp, Error, TEXT("No controller found"));
		return;
	}
	
	PC = Cast<AFTAPlayerController>(GetOwner()->GetInstigatorController());
	
	if(!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("No player controller found"));
		return;
	}

	//Need to check if this is okay to point to the ASC in controller, or should I get from PlayerState/PlayerCharacter
	if(!PC->ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("No ASC in player controller found"));
		return;
	}
	
	ASComponent = PC->ASC;
	
// 	FGameplayTag WindowTag;
// 	ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).AddLambda(
// 	[this, WindowTag](const FGameplayTag CallbackTag, int32 NewCount)
// 	{
// 		ComboWindowTagChanged(CallbackTag, NewCount, WindowTag);
// 	}
// );


}

void UPlayerComboManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPlayerComboManagerComponent::PrintCurrentComboContainer()
{
	for (const FGameplayTag& Tag : CurrentComboTagContainer)
	{
		if(*Tag.ToString())
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentComboTagContainer Tag: %s"), *Tag.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CurrentComboTagContainer Tag is NULL"));
		}
	}
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

void UPlayerComboManagerComponent::ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount,
	FGameplayTag WindowTag, FTimerHandle ComboWindowTimer)
{
	if (ASComponent->HasMatchingGameplayTag(WindowTag))
	{
		// Capture 'WindowTag' in the lambda
		GetWorld()->GetTimerManager().SetTimer(ComboWindowTimer, [this, WindowTag]()
		{
			ComboWindowOpen(WindowTag);  // Pass 'WindowTag' to ComboWindowOpen
		}, 0.01f, true);  // Set the interval to 0.01 seconds, looping every 0.01 seconds
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboWindowTimer);
	}
}

void UPlayerComboManagerComponent::ComboWindowOpen(FGameplayTag InputToCheck)
{
	UE_LOG(LogTemp, Warning, TEXT("ComboWindowOpen::LastInputSavedTag: %s"), *PC->LastInputSavedTag.GetTagName().ToString());
	
	if(PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light"))))
	{
		UE_LOG(LogTemp, Warning, TEXT("Call light attack"));
	}
	else if(PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Heavy"))))
	{
		UE_LOG(LogTemp, Warning, TEXT("Call heavy attack"));
	}
	else if(PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Dash"))))
	{
		UE_LOG(LogTemp, Warning, TEXT("Call Dash attack"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LastInputSavedTag doesnt match any"));
	}
}

void UPlayerComboManagerComponent::RegisterGameplayTagEvent(FGameplayTag InputSavedTag, FGameplayTag ComboWindow,
	FTimerHandle FComboWindowTimer)
{
	ASComponent->RegisterGameplayTagEvent(ComboWindow, EGameplayTagEventType::NewOrRemoved).AddLambda(
	[this, InputSavedTag, FComboWindowTimer](const FGameplayTag CallbackTag, int32 NewCount)
	{
		ComboWindowTagChanged(CallbackTag, NewCount, InputSavedTag, FComboWindowTimer);
	});

	UE_LOG(LogTemp, Warning, TEXT("InputSavedTag: %s"), *InputSavedTag.ToString());
	UE_LOG(LogTemp, Warning, TEXT("ComboWindow: %s"), *ComboWindow.ToString());
	UE_LOG(LogTemp, Warning, TEXT("FComboWindowTimer: %s"), *FComboWindowTimer.ToString());
}

