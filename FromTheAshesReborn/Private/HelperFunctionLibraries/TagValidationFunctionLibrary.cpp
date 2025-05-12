#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"

#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"

bool UTagValidationFunctionLibrary::IsRegisteredGameplayTag(const FGameplayTag& Tag)
{
	if (!Tag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("GameplayTag is not valid: TagName = '%s'"), *Tag.GetTagName().ToString());
		return false;
	}

	FGameplayTag FoundTag = UGameplayTagsManager::Get().RequestGameplayTag(Tag.GetTagName(), false);
	if (!FoundTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("GameplayTag is not registered in the system: TagName = '%s'"), *Tag.GetTagName().ToString());
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Test commit"));
	return true;

	
	
}