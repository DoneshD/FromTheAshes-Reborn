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
	if (!GetOwner()->GetInstigatorController())
	{
		UE_LOG(LogTemp, Error, TEXT("No controller found"));
		return;
	}
	
	PC = Cast<AFTAPlayerController>(GetOwner()->GetInstigatorController());
	
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("No player controller found"));
		return;
	}

	// Check if the ASC in the player controller is valid, or get it from PlayerState/PlayerCharacter if needed
	if (!PC->ASC)
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

void UPlayerComboManagerComponent::ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount, FGameplayTag ComboWindowTag)
{
	if (NewCount > 0 && ASComponent->HasMatchingGameplayTag(ComboWindowTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting timer for ComboWindowTag: %s"), *ComboWindowTag.ToString());

		// Start timer when the combo window tag is added
		FTimerHandle& ComboWindowTimer = TagTimerHandles.FindOrAdd(ComboWindowTag);
		GetWorld()->GetTimerManager().SetTimer(ComboWindowTimer, [this, ComboWindowTag]()
		{
			ComboWindowOpen(ComboWindowTag);
		}, 0.01f, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Removing tag and clearing timer for: %s"), *ComboWindowTag.ToString());

		// When the tag is removed or count is zero, clear the timer and event
		RemoveGameplayTagEvent(ComboWindowTag);
	}
}

void UPlayerComboManagerComponent::ComboWindowOpen(FGameplayTag ComboWindowTag)
{
    // UE_LOG(LogTemp, Warning, TEXT("ComboWindowOpen called for: %s"), *ComboWindowTag.ToString());

    if (PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light"))))
    {
        RemoveGameplayTagEvent(ComboWindowTag);
        ProceedNextAbility(7);
    }
    else if (PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Heavy"))))
    {
        RemoveGameplayTagEvent(ComboWindowTag);
        ProceedNextAbility(8);
    }
    else if (PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Dash"))))
    {
        UE_LOG(LogTemp, Warning, TEXT("CALLING DASH"));
        ProceedNextAbility(6);
    }
}

void UPlayerComboManagerComponent::ProceedNextAbility(int GameplayAbilityInputID)
{
    PC->LastInputSavedTag = FGameplayTag::RequestGameplayTag("Event.Input.Saved.None");
    ASComponent->CancelAllAbilities();

    for (const FGameplayAbilitySpec& Spec : ASComponent->GetActivatableAbilities())
    {
        if (Spec.Ability && Spec.InputID == GameplayAbilityInputID)
        {
            ASComponent->TryActivateAbility(Spec.Handle);
        }
    }
}

void UPlayerComboManagerComponent::RegisterGameplayTagEvent(FGameplayTag ComboWindowTag)
{
	if (!ASComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ASComponent is null in RegisterGameplayTagEvent"));
		return;
	}

	FDelegateHandle Handle = ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).AddLambda(
		[this, ComboWindowTag](const FGameplayTag CallbackTag, int32 NewCount)
	{
		ComboWindowTagChanged(CallbackTag, NewCount, ComboWindowTag);
	});
	
	TagDelegateHandles.Add(ComboWindowTag, Handle);
}


void UPlayerComboManagerComponent::RemoveGameplayTagEvent(FGameplayTag ComboWindowTag)
{
    // Clear the timer
    if (TagTimerHandles.Contains(ComboWindowTag))
    {
        FTimerHandle& ComboWindowTimer = TagTimerHandles[ComboWindowTag];
        if (GetWorld()->GetTimerManager().IsTimerActive(ComboWindowTimer))
        {
            GetWorld()->GetTimerManager().ClearTimer(ComboWindowTimer);
            UE_LOG(LogTemp, Warning, TEXT("Timer cleared for ComboWindowTag: %s"), *ComboWindowTag.ToString());
        }
        TagTimerHandles.Remove(ComboWindowTag);
    }

    // Unregister the delegate
    if (TagDelegateHandles.Contains(ComboWindowTag))
    {
        FDelegateHandle Handle = TagDelegateHandles[ComboWindowTag];
        ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).Remove(Handle);
        UE_LOG(LogTemp, Warning, TEXT("GameplayTag event unregistered for: %s"), *ComboWindowTag.ToString());
        TagDelegateHandles.Remove(ComboWindowTag);
    }
}
