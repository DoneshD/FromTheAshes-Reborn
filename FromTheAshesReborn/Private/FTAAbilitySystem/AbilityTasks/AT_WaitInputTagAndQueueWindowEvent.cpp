﻿#include "FTAAbilitySystem/AbilityTasks/AT_WaitInputTagAndQueueWindowEvent.h"
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
	
	for(FGameplayAbilitySpecHandle& Handle : SpecArray)
	{
		if(FGameplayAbilitySpec* Spec = FTAASC->FindAbilitySpecFromHandle(Handle))
		{
			if(UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(Spec->Ability))
			{
				if(FTAAbility->QueueWindowTag.IsValid())
				{
					QueueableAbilities.Add(FTAAbility->QueueWindowTag, FTAAbility);
					RegisterQueueWindowTagEvent(FTAAbility->QueueWindowTag);
					UE_LOG(LogTemp, Warning, TEXT("IRegisterQueueWindowTagEvent"))

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
		// if (UFTAGameplayAbility* FTAAbility = *QueueableAbilities.Find(QueueWindowTag))
		// {
		// 	FTAASC->ChangeActivationGroup(EFTAAbilityActivationGroup::Exclusive_Blocking, FTAAbility);
		// }
		TryActivateMatchingAbility(QueueWindowTag);
	}
	else
	{
		// if (UFTAGameplayAbility* FTAAbility = *QueueableAbilities.Find(QueueWindowTag))
		// {
		// 	FTAASC->ChangeActivationGroup(EFTAAbilityActivationGroup::Exclusive_Replaceable, FTAAbility);
		// }
	}
}

void UAT_WaitInputTagAndQueueWindowEvent::TryActivateMatchingAbility(const FGameplayTag& QueueWindowTag)
{
	if (UFTAGameplayAbility* FTAAbility = *QueueableAbilities.Find(QueueWindowTag))
	{
		if (FTAAbility && FTAAbility->InputTag.MatchesTag(QueuedInputTag))
		{
			QueuedInputTag = FGameplayTag::EmptyTag;
			FTAASC->CancelAllAbilities();
			bool IsActivated = FTAASC->TryActivateAbilityByClass(FTAAbility->GetClass());
			if (!IsActivated)
			{
				UE_LOG(LogTemp, Error, TEXT("TryActivateMatchingAbility: Activation failed for %s"), *FTAAbility->GetName());
			}
			else
			{
				EndTask();
			}
		}
	}
}
