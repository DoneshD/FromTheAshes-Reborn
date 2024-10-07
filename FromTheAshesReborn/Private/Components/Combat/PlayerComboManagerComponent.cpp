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
	FGameplayTag ComboWindowTag, FTimerHandle& ComboWindowTimer)
{
	UE_LOG(LogTemp, Error, TEXT("UPlayerComboManagerComponent FComboWindowTimer: %s"), ComboWindowTimer.IsValid() ? TEXT("true") : TEXT("false"));

	if (ASComponent->HasMatchingGameplayTag(ComboWindowTag))
	{
		GetWorld()->GetTimerManager().SetTimer(ComboWindowTimer, [this, ComboWindowTag]()
		{
			ComboWindowOpen(ComboWindowTag);  
		}, 0.01f, true);  
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Clear Timer"))
		GetWorld()->GetTimerManager().ClearTimer(ComboWindowTimer);
	}
}

void UPlayerComboManagerComponent::ComboWindowOpen(FGameplayTag ComboWindowTag)
{
	if(PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light"))))
	{
		PC->LastInputSavedTag = FGameplayTag::RequestGameplayTag("Event.Input.Saved.None");
		ASComponent->CancelAllAbilities();

		UE_LOG(LogTemp, Warning, TEXT("Call light attack"));
		for (FGameplayAbilitySpec& Spec : ASComponent->GetActivatableAbilities())
		{
			if (Spec.Ability)
			{
				if(Spec.InputID == 7)
				{
					ASComponent->TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
	else if(PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Heavy"))))
	{
		PC->LastInputSavedTag = FGameplayTag::RequestGameplayTag("Event.Input.Saved.None");
		ASComponent->CancelAllAbilities();
		for (FGameplayAbilitySpec& Spec : ASComponent->GetActivatableAbilities())
		{
			if (Spec.Ability)
			{
				if(Spec.InputID == 8)
				{
					ASComponent->TryActivateAbility(Spec.Handle);
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Call heavy attack"));
	}
	else if(PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Dash"))))
	{
		UE_LOG(LogTemp, Warning, TEXT("Call Dash attack"));
	}
	else
	{
		// UE_LOG(LogTemp, Error, TEXT("LastInputSavedTag doesnt match any"));
	}
}

void UPlayerComboManagerComponent::RegisterGameplayTagEvent(FGameplayTag ComboWindowTag,
	FTimerHandle& FComboWindowTimer)
{
	ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).AddLambda(
	[this, ComboWindowTag, &FComboWindowTimer](const FGameplayTag CallbackTag, int32 NewCount)
	{
		ComboWindowTagChanged(CallbackTag, NewCount, ComboWindowTag, FComboWindowTimer);
	});
	
}

void UPlayerComboManagerComponent::RemoveGameplayTagEvent(FGameplayTag ComboWindowTag)
{

	ASComponent->RegisterGameplayTagEvent(ComboWindowTag,
			EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
}
