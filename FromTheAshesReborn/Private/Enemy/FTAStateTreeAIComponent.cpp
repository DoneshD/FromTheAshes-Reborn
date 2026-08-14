#include "Enemy/FTAStateTreeAIComponent.h"
#include "Components/StateTreeComponent.h"
#include "StateTree.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"


UFTAStateTreeAIComponent::UFTAStateTreeAIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UFTAStateTreeAIComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UFTAStateTreeAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UFTAStateTreeAIComponent::SetStateTree(UStateTree* InStateTree)
{
	if(IsRunning())
	{
		Cleanup();
	}
	StateTreeRef.SetStateTree(InStateTree);
	
}

void UFTAStateTreeAIComponent::AddLinkedStateTreeOverrides(
	FGameplayTag SubTreeAssetTag,
	UStateTree* InStateTree)
{
	if (!UTagValidationFunctionLibrary::IsRegisteredGameplayTag(SubTreeAssetTag))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("UFTAStateTreeAIComponent::AddLinkedStateTreeOverrides - Invalid tag")
		);
		return;
	}

	if (!IsValid(InStateTree))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("UFTAStateTreeAIComponent::AddLinkedStateTreeOverrides - Invalid tree")
		);
		return;
	}

	FStateTreeReference TreeRef;
	TreeRef.SetStateTree(InStateTree);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Linked State Tree Override - Tag: %s | Input Tree: %s | TreeRef: %s"),
		*SubTreeAssetTag.ToString(),
		*GetNameSafe(InStateTree),
		*GetNameSafe(TreeRef.GetStateTree())
	);

	LinkedStateTreeOverrides.AddOverride(
		SubTreeAssetTag,
		TreeRef
	);

}

