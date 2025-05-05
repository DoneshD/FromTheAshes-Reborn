#include "FTAAbilitySystem/AbilityTasks/AT_WaitInputTagAndQueueWindowEvent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

UAT_WaitInputTagAndQueueWindowEvent::UAT_WaitInputTagAndQueueWindowEvent(const FObjectInitializer& ObjectInitializer)
{
	
}

UAT_WaitInputTagAndQueueWindowEvent* UAT_WaitInputTagAndQueueWindowEvent::WaitInputTagAndQueueWindowEvent(UGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UAT_WaitInputTagAndQueueWindowEvent>(OwningAbility);
}

void UAT_WaitInputTagAndQueueWindowEvent::Activate()
{
	Super::Activate();
	
	FTAASC = Cast<UFTAAbilitySystemComponent>(AbilitySystemComponent);
	if(!FTAASC || !Ability)
	{
		UE_LOG(LogTemp, Error, TEXT("UAT_WaitInputTagAndQueueWindowEvent::Activate - FTAASC or Ability is invalid"));
		return;
	}
	
	FTAASC->OnInputQueueReceived.AddDynamic(this, &UAT_WaitInputTagAndQueueWindowEvent::OnInputTagReceived);

	TArray<FGameplayAbilitySpecHandle> SpecArray;
	FTAASC->GetAllAbilities(SpecArray);

	for (FGameplayAbilitySpecHandle& Handle : SpecArray)
	{
		if (FGameplayAbilitySpec* Spec = FTAASC->FindAbilitySpecFromHandle(Handle))
		{
			if (UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(Spec->Ability))
			{
				if (FTAAbility->QueueWindowTag.IsValid())
				{
					TArray<UFTAGameplayAbility*>& Abilities = QueueableAbilities.FindOrAdd(FTAAbility->QueueWindowTag);
				
					if (!Abilities.Contains(FTAAbility))
					{
						Abilities.Add(FTAAbility);
					}
					RegisterQueueWindowTagEvent(FTAAbility->QueueWindowTag);
				}
			}
		}
	}
}

void UAT_WaitInputTagAndQueueWindowEvent::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UAT_WaitInputTagAndQueueWindowEvent::GetDebugString() const
{
	return Super::GetDebugString();
}

void UAT_WaitInputTagAndQueueWindowEvent::OnDestroy(bool AbilityEnded)
{
	if (FTAASC)
	{
		FTAASC->OnInputQueueReceived.RemoveDynamic(this, &UAT_WaitInputTagAndQueueWindowEvent::OnInputTagReceived);
		for (const auto& Pair : QueueWindowHandles)
		{
			FTAASC->RegisterGameplayTagEvent(Pair.Key, EGameplayTagEventType::NewOrRemoved)
				.Remove(Pair.Value.DelegateHandle);
		}
		QueueWindowHandles.Empty();
		FTAASC->RemoveLooseGameplayTag(QueuedInputTag);

	}
	Super::OnDestroy(AbilityEnded);
}

void UAT_WaitInputTagAndQueueWindowEvent::OnInputTagReceived(FGameplayTag InputTag)
{
	QueuedInputTag = InputTag;
	for (const auto& Pair : QueueableAbilities)
	{
		if (FTAASC->HasMatchingGameplayTag(Pair.Key))
		{
			TryActivateMatchingAbility(Pair.Key);
		}
	}
}

void UAT_WaitInputTagAndQueueWindowEvent::RegisterQueueWindowTagEvent(FGameplayTag QueueWindowTag)
{
	RemoveQueueWindowTagEvent(QueueWindowTag);

	FDelegateHandle Handle = FTAASC->RegisterGameplayTagEvent(QueueWindowTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UAT_WaitInputTagAndQueueWindowEvent::OnQueueWindowTagChanged);
	QueueWindowHandles.Add(QueueWindowTag, {Handle});
}

void UAT_WaitInputTagAndQueueWindowEvent::RemoveQueueWindowTagEvent(FGameplayTag QueueWindowTag)
{
	if (FQueueWindowHandle* Handle = QueueWindowHandles.Find(QueueWindowTag))
	{
		FTAASC->RegisterGameplayTagEvent(QueueWindowTag, EGameplayTagEventType::NewOrRemoved)
			.Remove(Handle->DelegateHandle);
		QueueWindowHandles.Remove(QueueWindowTag);
	}
}

void UAT_WaitInputTagAndQueueWindowEvent::OnQueueWindowTagChanged(const FGameplayTag QueueWindowTag, int32 NewCount)
{
	if (NewCount > 0 && FTAASC->HasMatchingGameplayTag(QueueWindowTag))
	{
		TArray<UFTAGameplayAbility*> Abilities = *QueueableAbilities.Find(QueueWindowTag);

		for(UFTAGameplayAbility* FTAAbility : Abilities)
		{
			if (FTAAbility)
			{
				FTAASC->ChangeActivationGroup(EFTAAbilityActivationGroup::Exclusive_Replaceable, FTAAbility);
			}
		}
		
		TryActivateMatchingAbility(QueueWindowTag);
	}
	else
	{
		TArray<UFTAGameplayAbility*> Abilities = *QueueableAbilities.Find(QueueWindowTag);

		for(UFTAGameplayAbility* FTAAbility : Abilities)
		{
			if (FTAAbility)
			{
				FTAASC->ChangeActivationGroup(EFTAAbilityActivationGroup::Exclusive_Blocking, FTAAbility);
			}
		}
	}
}

void UAT_WaitInputTagAndQueueWindowEvent::TryActivateMatchingAbility(const FGameplayTag& QueueWindowTag)
{
	if (TArray<UFTAGameplayAbility*>* FTAAbilities = QueueableAbilities.Find(QueueWindowTag))
	{
		for (UFTAGameplayAbility* FTAAbility : *FTAAbilities)
		{
			if (FTAAbility && FTAAbility->InputTag.MatchesTag(QueuedInputTag))
			{
				if (FTAASC->IsAbilityActive(FTAAbility->GetClass()))
				{
					FTAASC->CancelAbilityByClass(FTAAbility->GetClass());
				}

				bool bIsActivated = FTAASC->TryActivateAbilityByClass(FTAAbility->GetClass());
				if (bIsActivated)
				{
					QueuedInputTag = FGameplayTag::EmptyTag;
					EndTask();
				}
			}
		}
	}
}

