
#include "FTAAbilitySystem/AsyncTasks/AsyncTaskGameplayTagAddedRemoved.h"

UAsyncTaskGameplayTagAddedRemoved* UAsyncTaskGameplayTagAddedRemoved::ListenForGameplayTagAddedOrRemoved(UAbilitySystemComponent * AbilitySystemComponent, FGameplayTagContainer InTags)
{
	UAsyncTaskGameplayTagAddedRemoved* ListenForGameplayTagAddedRemoved = NewObject<UAsyncTaskGameplayTagAddedRemoved>();
	ListenForGameplayTagAddedRemoved->ASC = AbilitySystemComponent;
	ListenForGameplayTagAddedRemoved->Tags = InTags;

	if (!IsValid(AbilitySystemComponent) || InTags.Num() < 1)
	{
		ListenForGameplayTagAddedRemoved->EndTask();
		return nullptr;
	}

	TArray<FGameplayTag> TagArray;
	InTags.GetGameplayTagArray(TagArray);
	
	for (FGameplayTag Tag : TagArray)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).AddUObject(ListenForGameplayTagAddedRemoved, &UAsyncTaskGameplayTagAddedRemoved::TagChanged);
	}
	UE_LOG(LogTemp, Warning, TEXT("ListenForGameplayTagAddedRemoved"));

	return ListenForGameplayTagAddedRemoved;
}

void UAsyncTaskGameplayTagAddedRemoved::EndTask()
{
	if (IsValid(ASC))
	{
		TArray<FGameplayTag> TagArray;
		Tags.GetGameplayTagArray(TagArray);

		for (FGameplayTag Tag : TagArray)
		{
			ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
			UE_LOG(LogTemp, Warning, TEXT("EndTask"));

		}
	}

	SetReadyToDestroy();
	//MarkPendingKill();
}

void UAsyncTaskGameplayTagAddedRemoved::TagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		OnTagAdded.Broadcast(Tag);
		UE_LOG(LogTemp, Warning, TEXT("OnTagAdded"));
	}
	else
	{
		OnTagRemoved.Broadcast(Tag);
		UE_LOG(LogTemp, Warning, TEXT("OnTagRemoved"));

	}
}