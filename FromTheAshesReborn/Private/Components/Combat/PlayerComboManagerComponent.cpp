#include "Components/Combat/PlayerComboManagerComponent.h"
#include "AbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedLightMeleeAttack.h"
#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedHeavyMeleeAttack.h"
#include "FTAAbilitySystem/GameplayAbilities/GA_GroundedDash.h"

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
	LightAbilityData = FAbilityComboData{ GA_GroundedLightMeleeAttack, 
	FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Light")),
	FGameplayTag::RequestGameplayTag(FName("Event.Montage.ComboWindow.Open.Light")) };

	HeavyAbilityData = FAbilityComboData{ GA_GroundedHeavyMeleeAttack, 
		FGameplayTag::RequestGameplayTag(FName("Event.Input.Saved.Heavy")),
		FGameplayTag::RequestGameplayTag(FName("Event.Montage.ComboWindow.Open.Heavy")) };
	
	AbilityComboDataArray.Add(LightAbilityData);
	AbilityComboDataArray.Add(HeavyAbilityData);
	
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
	for (const FAbilityComboData& AbilityComboData : AbilityComboDataArray)
	{
		if (PC->LastInputSavedTag.MatchesTag(AbilityComboData.InputSavedTag) && ComboWindowTag.MatchesTag(AbilityComboData.ComboWindowTag))
		{
			ProceedToNextAbility(AbilityComboData.AbilityComboClass);
		}
	}
}

void UPlayerComboManagerComponent::ProceedToNextAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	PC->LastInputSavedTag = FGameplayTag::RequestGameplayTag("Event.Input.Saved.None");
	ASComponent->CancelAllAbilities();
	
	bool bActivateAbility = ASComponent->TryActivateAbilityByClass(AbilityClass);
	if(!bActivateAbility)
	{
		UE_LOG(LogTemp, Error, TEXT("ProceedToNextAbility: Ability did not activate"));
	}
}

void UPlayerComboManagerComponent::RegisterGameplayTagEvent(FAbilityComboData AbilityComboData)
{
	if (!ASComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ASComponent is null in RegisterGameplayTagEvent"));
		return;
	}

	RemoveGameplayTagEvent(AbilityComboData.ComboWindowTag);
	
	FDelegateHandle Handle = ASComponent->RegisterGameplayTagEvent(AbilityComboData.ComboWindowTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UPlayerComboManagerComponent::ComboWindowTagChanged);
	TagDelegateHandles.Add(AbilityComboData.ComboWindowTag, Handle);
}


void UPlayerComboManagerComponent::RemoveGameplayTagEvent(FGameplayTag ComboWindowTag)
{
    if (TagTimerHandles.Contains(ComboWindowTag))
    {
        FTimerHandle& ComboWindowTimer = TagTimerHandles[ComboWindowTag];
        if (GetWorld()->GetTimerManager().IsTimerActive(ComboWindowTimer))
        {
            GetWorld()->GetTimerManager().ClearTimer(ComboWindowTimer);
            // UE_LOG(LogTemp, Warning, TEXT("Timer cleared for ComboWindowTag: %s"), *ComboWindowTag.ToString());
        }
        
        TagTimerHandles.Remove(ComboWindowTag);
    }
	
    if (TagDelegateHandles.Contains(ComboWindowTag))
    {
        FDelegateHandle Handle = TagDelegateHandles[ComboWindowTag];
        ASComponent->RegisterGameplayTagEvent(ComboWindowTag, EGameplayTagEventType::NewOrRemoved).Remove(Handle);
        // UE_LOG(LogTemp, Warning, TEXT("GameplayTag event unregistered for: %s"), *ComboWindowTag.ToString());
        TagDelegateHandles.Remove(ComboWindowTag);
    }
    
}