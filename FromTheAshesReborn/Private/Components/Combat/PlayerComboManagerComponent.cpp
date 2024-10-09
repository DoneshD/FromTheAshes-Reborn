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

void UPlayerComboManagerComponent::ComboWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount,
                                                         FGameplayTag ComboWindowTag, FTimerHandle& ComboWindowTimer)
{
	if (ASComponent->HasMatchingGameplayTag(ComboWindowTag))
	{
		GetWorld()->GetTimerManager().SetTimer(ComboWindowTimer, [this, ComboWindowTag, &ComboWindowTimer]()
		{
			ComboWindowOpen(ComboWindowTag, ComboWindowTimer);
		}, 0.01f, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CLEAR TIMER"));
		GetWorld()->GetTimerManager().ClearTimer(ComboWindowTimer);
	}
}

void UPlayerComboManagerComponent::ComboWindowOpen(FGameplayTag ComboWindowTag, FTimerHandle& ComboWindowTimer)
{
	UE_LOG(LogTemp, Error, TEXT("IM OPEN with: %s"), *ComboWindowTag.GetTagName().ToString());

	if (PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light"))))
	{
		ProceedNextAbility(7);
	}
	else if (PC->LastInputSavedTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Heavy"))))
	{
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

	for (FGameplayAbilitySpec& Spec : ASComponent->GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.InputID == GameplayAbilityInputID)
		{
			ASComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UPlayerComboManagerComponent::RegisterGameplayTagEvent(FGameplayTag& ComboWindowTag, FTimerHandle& FComboWindowTimer)
{
	FDelegateHandle Handle = ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).AddLambda(
		[this, ComboWindowTag, &FComboWindowTimer](const FGameplayTag CallbackTag, int32 NewCount)
	{
		ComboWindowTagChanged(CallbackTag, NewCount, ComboWindowTag, FComboWindowTimer);
	});
	
	TagEventHandles.Add(ComboWindowTag, Handle);
}

void UPlayerComboManagerComponent::RemoveGameplayTagEvent(FGameplayTag& ComboWindowTag, FTimerHandle& ComboWindowTimer)
{
	if (FDelegateHandle* Handle = TagEventHandles.Find(ComboWindowTag))
	{
		ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
		ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).Remove(*Handle);
		ASComponent->UnregisterGameplayTagEvent(*Handle, ComboWindowTag);

		UE_LOG(LogTemp, Warning, TEXT("FGDSGDSGSDGSDGG"))
		TagEventHandles.Remove(ComboWindowTag);
	}
}

