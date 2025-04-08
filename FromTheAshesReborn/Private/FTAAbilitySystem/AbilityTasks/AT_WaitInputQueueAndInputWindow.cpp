#include "FTAAbilitySystem/AbilityTasks/AT_WaitInputQueueAndInputWindow.h"

#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

UAT_WaitInputQueueAndInputWindow::UAT_WaitInputQueueAndInputWindow(const FObjectInitializer& ObjectInitializer)
{
	
}

UAT_WaitInputQueueAndInputWindow* UAT_WaitInputQueueAndInputWindow::WaitInputQueueAndInputWindow(UGameplayAbility* OwningAbility)
{
	UAT_WaitInputQueueAndInputWindow* Obj = NewAbilityTask<UAT_WaitInputQueueAndInputWindow>(OwningAbility);
	
	return Obj;
}

void UAT_WaitInputQueueAndInputWindow::Activate()
{
	Super::Activate();
	if (!Ability)
	{
		UE_LOG(LogTemp, Error, TEXT("UAT_WaitQueueInput - Ability is invalid"))
		return;
	}

	FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent);

	if (FTAASC)
	{
		FTAASC->OnInputQueueReceived.AddDynamic(this, &UAT_WaitInputQueueAndInputWindow::OnInputQueueTagReceived);
		OnRegisterInputWindowTagEventDelegate.AddUniqueDynamic(this, &UAT_WaitInputQueueAndInputWindow::RegisterInputWindowTagEvent);
		
	}
	
	for (const FGameplayAbilitySpec& Spec : FTAASC->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(Spec.Ability);
			if(FTAAbility->ComboWindowTag.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("Found ability: %s"), *Spec.Ability->GetName())
				OnRegisterInputWindowTagEventDelegate.Broadcast(FTAAbility->ComboWindowTag);
			}
		}
	}
}

void UAT_WaitInputQueueAndInputWindow::OnInputQueueTagReceived(UFTAAbilitySystemComponent* SourceASC,FGameplayTag InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("OnInputReceived"))
	UE_LOG(LogTemp, Warning, TEXT("Tag Name: %s"), *InputTag.GetTagName().ToString());
	CurrentInputTag = InputTag;
}

void UAT_WaitInputQueueAndInputWindow::ChangeActivationGroupToReplaceable(FGameplayTag InputWindowTag)
{
	if(FTAASC)
	{
		for (const FGameplayAbilitySpec& Spec : FTAASC->GetActivatableAbilities())
		{
			if (Spec.Ability)
			{
				UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(Spec.Ability);
				if(FTAASC)
				{
					if(FTAAbility->ComboWindowTag.MatchesTag(InputWindowTag))
					{
						UE_LOG(LogTemp, Log, TEXT("Found ability to try and change group: %s"), *Spec.Ability->GetName());

						FTAASC->ChangeActivationGroup(EFTAAbilityActivationGroup::Exclusive_Replaceable, FTAAbility);
						
					}
				}
			}
		}
	}
}

void UAT_WaitInputQueueAndInputWindow::ChangeActivationGroupToBlocking(FGameplayTag InputWindowTag)
{
	if(FTAASC)
	{
		for (const FGameplayAbilitySpec& Spec : FTAASC->GetActivatableAbilities())
		{
			if (Spec.Ability)
			{
				UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(Spec.Ability);
				if(FTAASC)
				{
					if(FTAAbility->ComboWindowTag.MatchesTag(InputWindowTag))
					{
						UE_LOG(LogTemp, Log, TEXT("Found ability to try and change group: %s"), *Spec.Ability->GetName());

						FTAASC->ChangeActivationGroup(EFTAAbilityActivationGroup::Exclusive_Blocking, FTAAbility);
						
					}
				}
			}
		}
	}
}

void UAT_WaitInputQueueAndInputWindow::RegisterInputWindowTagEvent(FGameplayTag InputWindowTag)
{
	RemoveInputWindowTagEvent(InputWindowTag);
	
	FDelegateHandle Handle = FTAASC->RegisterGameplayTagEvent(InputWindowTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UAT_WaitInputQueueAndInputWindow::InputWindowTagChanged);
	TagDelegateHandles.Add(InputWindowTag, Handle);

}

void UAT_WaitInputQueueAndInputWindow::InputWindowTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Tag changed"))
	if (NewCount > 0 && FTAASC->HasMatchingGameplayTag(CallbackTag))
	{
		ChangeActivationGroupToReplaceable(CallbackTag);
		FTimerHandle& ComboWindowTimer = TagTimerHandles.FindOrAdd(CallbackTag);
		GetWorld()->GetTimerManager().SetTimer(ComboWindowTimer, [this, CallbackTag]()
		{
			InputWindowOpen(CallbackTag);
			
		}, 0.01f, true);
	}
	else
	{
		ChangeActivationGroupToBlocking(CallbackTag);
		RemoveInputWindowTagEvent(CallbackTag);
	}
}

void UAT_WaitInputQueueAndInputWindow::InputWindowOpen(FGameplayTag InputWindowTag)
{
	// UE_LOG(LogTemp, Warning, TEXT("InputWindowOpen with InputWindowTag: %s"), *InputWindowTag.GetTagName().ToString());
	// UE_LOG(LogTemp, Warning, TEXT("InputWindowOpen with InputWindowTag: %s"), *InputWindowTag.GetTagName().ToString());

	if(FTAASC)
	{
		for (const FGameplayAbilitySpec& Spec : FTAASC->GetActivatableAbilities())
		{
			if (Spec.Ability)
			{
				UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(Spec.Ability);
				if(FTAASC)
				{
					if(FTAAbility->ComboWindowTag.MatchesTag(InputWindowTag) && FTAAbility->InputTag.MatchesTag(CurrentInputTag))
					{
						
						CurrentInputTag = FGameplayTag::EmptyTag;
						InputWindowTag = FGameplayTag::EmptyTag;
						FTAASC->CancelAllAbilities();
						
						bool bActivateAbility = FTAASC->TryActivateAbilityByClass(FTAAbility->GetClass());
			
						if(!bActivateAbility)
						{
							UE_LOG(LogTemp, Error, TEXT("ProceedToNextAbility: Ability did not activate"));
						}
					}
				}
			}
		}
	}
}

void UAT_WaitInputQueueAndInputWindow::ProceedToNextAbility()
{
	
}

void UAT_WaitInputQueueAndInputWindow::RemoveInputWindowTagEvent(FGameplayTag InputWindowTag)
{
	if (TagTimerHandles.Contains(InputWindowTag))
	{
		FTimerHandle& ComboWindowTimer = TagTimerHandles[InputWindowTag];
		if (GetWorld()->GetTimerManager().IsTimerActive(ComboWindowTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(ComboWindowTimer);
		}
		TagTimerHandles.Remove(InputWindowTag);
	}
	
	if (TagDelegateHandles.Contains(InputWindowTag))
	{
		FDelegateHandle Handle = TagDelegateHandles[InputWindowTag];
		FTAASC->RegisterGameplayTagEvent(InputWindowTag, EGameplayTagEventType::NewOrRemoved).Remove(Handle);
		TagDelegateHandles.Remove(InputWindowTag);
	}
}

void UAT_WaitInputQueueAndInputWindow::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UAT_WaitInputQueueAndInputWindow::GetDebugString() const
{
	return Super::GetDebugString();
}

void UAT_WaitInputQueueAndInputWindow::OnDestroy(bool AbilityEnded)
{

	FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent);

	if (FTAASC)
	{
		FTAASC->OnInputQueueReceived.RemoveDynamic(this, &UAT_WaitInputQueueAndInputWindow::OnInputQueueTagReceived);
	}

	Super::OnDestroy(AbilityEnded);

}

