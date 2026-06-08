#include "FTAAbilitySystem/AbilityTasks/AT_WaitInputTagAndQueueWindowEvent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"

UAT_WaitInputTagAndQueueWindowEvent::UAT_WaitInputTagAndQueueWindowEvent(const FObjectInitializer& ObjectInitializer)
{
	
}

UAT_WaitInputTagAndQueueWindowEvent* UAT_WaitInputTagAndQueueWindowEvent::WaitInputTagAndQueueWindowEvent(UGameplayAbility* OwningAbility)
{
	UAT_WaitInputTagAndQueueWindowEvent* Task = NewAbilityTask<UAT_WaitInputTagAndQueueWindowEvent>(OwningAbility);
	return Task;
}

void UAT_WaitInputTagAndQueueWindowEvent::Activate()
{
	Super::Activate();
	
	AFTAPlayerState* PS = Cast<AFTAPlayerState>(GetOwnerActor());
	
	if (!PS)
	{
		return;
	}
	
	FTAASC = PS->GetFTAAbilitySystemComponent();
	
	if (!FTAASC || !Ability)
	{
		UE_LOG(LogTemp, Error, TEXT("UAT_WaitInputTagAndQueueWindowEvent::Activate - FTAASC || Ability is Null"));
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
				if (UTagValidationFunctionLibrary::IsRegisteredGameplayTag(FTAAbility->QueueWindowTag))
				{
					if(FTAAbility->IsInputQueueable)
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
}

void UAT_WaitInputTagAndQueueWindowEvent::OnInputTagReceived(FGameplayTag InputTag)
{
	if(!InputTag.MatchesTag(FGameplayTag::RequestGameplayTag("InputTag.Pressed.Hold.Special")))
	{
		QueuedInputData.InputTag = InputTag;
	}
	
	AFTAPlayerState* PS = Cast<AFTAPlayerState>(Ability->GetActorInfo().OwnerActor);
	if (!PS)
	{
		UE_LOG(LogTemp, Error, TEXT("NULL PS"));
		return;
	}
	
	if(PS->HardLockedTargetActor)
	{
		FLockOnAngleResult AngleResult = ULockOnFunctionLibrary::AngleFromInputVectorToLockedTarget(Ability->GetAvatarActorFromActorInfo(), PS->HardLockedTargetActor);
		ELockOnInputOrientationDirection InputDirection = ULockOnFunctionLibrary::GetOrientationOfInput(AngleResult);
		QueuedInputData.Direction = InputDirection;
	}
	
	TArray<FAbilityCandidate> Candidates;
	
	for (const auto& Pair : QueueableAbilities)
	{
		const FGameplayTag& WindowTag = Pair.Key;
		
		if (FTAASC->HasMatchingGameplayTag(WindowTag))
		{
			bool bHasTag = FTAASC->HasMatchingGameplayTag(WindowTag);
			TArray<UFTAGameplayAbility*>* AbilitiesPtr = QueueableAbilities.Find(WindowTag);
			if (!AbilitiesPtr) continue;

			for (UFTAGameplayAbility* FTAAbility : *AbilitiesPtr)
			{
				if(!FTAAbility)
				{
					continue;
				}
				
				if(!FTAAbility->IsInputQueueable)
				{
					continue;
				}
				
				if (!FTAAbility->InputTag.MatchesTag(QueuedInputData.InputTag))
				{
					continue;
				}

				if(FTAAbility->IsLockOnDirectionalInput)
				{
					if(FTAAbility->LockOnInputDirection != QueuedInputData.Direction)
					{
						continue;
					}
				}
				
				// if (FTAASC->IsAbilityActive(FTAAbility->GetClass()))
				// {
				// 	FTAASC->CancelAbilityByClass(FTAAbility->GetClass());
				// }

				FAbilityCandidate Candidate;
				Candidate.Ability = FTAAbility;
				Candidate.Priority = FTAAbility->ActivationPriority;

				Candidates.Add(Candidate);
			}
		}
	}
	TryActivateBestCandidate(Candidates);
}


void UAT_WaitInputTagAndQueueWindowEvent::RegisterQueueWindowTagEvent(FGameplayTag QueueWindowTag)
{
	RemoveQueueWindowTagEvent(QueueWindowTag);

	FDelegateHandle Handle = FTAASC->RegisterGameplayTagEvent(QueueWindowTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UAT_WaitInputTagAndQueueWindowEvent::OnQueueWindowTagChanged);

	QueueWindowHandles.Add(QueueWindowTag, { Handle });
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
	if (!FTAASC)
	{
		return;
	}

	TArray<UFTAGameplayAbility*>* AbilitiesPtr = QueueableAbilities.Find(QueueWindowTag);
	if (!AbilitiesPtr) return;

	if (NewCount > 0 && FTAASC->HasMatchingGameplayTag(QueueWindowTag))
	{
		TArray<FAbilityCandidate> Candidates;
		
		for (UFTAGameplayAbility* FTAAbility : *AbilitiesPtr)
		{
			if(!FTAAbility)
			{
				continue;
			}
				
			if(!FTAAbility->IsInputQueueable)
			{
				continue;
			}
				
			if (!FTAAbility->InputTag.MatchesTag(QueuedInputData.InputTag))
			{
				continue;
			}

			if(FTAAbility->IsLockOnDirectionalInput)
			{
				if(FTAAbility->LockOnInputDirection != QueuedInputData.Direction)
				{
					continue;
				}
			}
				
			// if (FTAASC->IsAbilityActive(FTAAbility->GetClass()))
			// {
			// 	FTAASC->CancelAbilityByClass(FTAAbility->GetClass());
			// }
			
			// FTAASC->ChangeToActivationGroup(FGameplayTag::RequestGameplayTag("ActivationGroupTag.Exclusive.Replaceable"), FTAAbility);

			FAbilityCandidate Candidate;
			Candidate.Ability = FTAAbility;
			Candidate.Priority = FTAAbility->ActivationPriority;

			Candidates.Add(Candidate);
		}
		TryActivateBestCandidate(Candidates);
		
	}
	else
	{
		for (UFTAGameplayAbility* FTAAbility : *AbilitiesPtr)
		{
			if (FTAAbility)
			{
				FTAASC->ChangeToActivationGroup(FGameplayTag::RequestGameplayTag("ActivationGroupTag.Exclusive.Blocking"), FTAAbility);
			}
		}
	}
}

bool UAT_WaitInputTagAndQueueWindowEvent::TryActivateBestCandidate(TArray<FAbilityCandidate>& Candidates)
{
	if (Candidates.Num() == 0)
	{
		return false;
	}

	Candidates.Sort([](const FAbilityCandidate& A, const FAbilityCandidate& B)
	{
		return A.Priority > B.Priority;
	});

	for(auto Element : Candidates)
	{
		if(Element.Ability)
		{
			FTAASC->ChangeToActivationGroup(FGameplayTag::RequestGameplayTag("ActivationGroupTag.Exclusive.Replaceable"), Element.Ability);

			if (FTAASC->IsAbilityActive(Element.Ability->GetClass()))
			{
				FTAASC->CancelAbilityByClass(Element.Ability->GetClass());
			}

			bool bActivated = FTAASC->TryActivateAbilityByClass(Element.Ability->GetClass());

			if (bActivated)
			{
				QueuedInputData.InputTag = FGameplayTag::EmptyTag;
				EndTask();
				return true;
			}
		}
		
	}
	

	return false;
}

void UAT_WaitInputTagAndQueueWindowEvent::ExternalCancel()
{
	Super::ExternalCancel();
}

FString UAT_WaitInputTagAndQueueWindowEvent::GetDebugString() const
{
	FString DebugString = TEXT("WaitInputTagAndQueueWindowEvent: ");

	if (!QueuedInputData.InputTag.IsValid())
	{
		DebugString += TEXT("Waiting for any input tag");
	}
	else
	{
		DebugString += FString::Printf(TEXT("Waiting for input tag: %s"), *QueuedInputData.InputTag.ToString());
	}

	return DebugString;
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
		FTAASC->RemoveLooseGameplayTag(QueuedInputData.InputTag);
	}
	
	Super::OnDestroy(AbilityEnded);
}
