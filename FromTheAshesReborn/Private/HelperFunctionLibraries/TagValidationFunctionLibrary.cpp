#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

//TODO: Change to stack trace rather than log state

bool UTagValidationFunctionLibrary::IsRegisteredGameplayTag(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())
	{
		// UE_LOG(LogTemp, Error, TEXT("UTagValidationFunctionLibrary::IsRegisteredGameplayTag - GameplayTag is not valid: TagName = '%s'"), *Tag.GetTagName().ToString());
		return false;
	}

	if (FGameplayTag FoundTag = UGameplayTagsManager::Get().RequestGameplayTag(Tag.GetTagName(), false); !FoundTag.IsValid())
	{
		// UE_LOG(LogTemp, Error, TEXT("UTagValidationFunctionLibrary::IsRegisteredGameplayTag - GameplayTag is not registered in the system: TagName = '%s'"), *Tag.GetTagName().ToString());
		return false;
	}

	return true;

}

bool UTagValidationFunctionLibrary::AllGameplayTagsRegisteredInContainer(const FGameplayTagContainer& Container)
{
	for(const FGameplayTag& Tag: Container)
	{
		if (!Tag.IsValid())
		{
			// UE_LOG(LogTemp, Error, TEXT("UTagValidationFunctionLibrary::AllGameplayTagsRegisteredInContainer - GameplayTag is not valid: TagName = '%s'"), *Tag.GetTagName().ToString());
			return false;
		}

		if (FGameplayTag FoundTag = UGameplayTagsManager::Get().RequestGameplayTag(Tag.GetTagName(), false); !FoundTag.IsValid())
		{
			// UE_LOG(LogTemp, Error, TEXT("UTagValidationFunctionLibrary::AllGameplayTagsRegisteredInContainer - GameplayTag is not registered in the system: TagName = '%s'"), *Tag.GetTagName().ToString());
			return false;
		}
	}
	return true;
}
