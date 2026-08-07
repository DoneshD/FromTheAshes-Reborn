#include "StateTree/Evaluators/STE_SelectBestAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

void FStateTreeEvaluator_SelectBestAbility::TreeStart(FStateTreeExecutionContext& Context) const
{
	UE_LOG(LogTemp, Warning, TEXT("HERE 1"));
	
	FStateTreeEvaluatorCommonBase::TreeStart(Context);

	UE_LOG(LogTemp, Warning, TEXT("HERE 2"));

	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.OwningActor)
	{
		UE_LOG(LogTemp, Error, TEXT("OwningActor is null"))
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstanceData.OwningActor);
	if (ASC)
	{
		if (UFTAAbilitySystemComponent* FTAASC = Cast<UFTAAbilitySystemComponent>(ASC))
		{
			TArray<FGameplayAbilitySpecHandle> SpecArray;
			FTAASC->GetAllAbilities(SpecArray);

			for (FGameplayAbilitySpecHandle& Handle : SpecArray)
			{
				if (FGameplayAbilitySpec* Spec = FTAASC->FindAbilitySpecFromHandle(Handle))
				{
					if (UFTAGameplayAbility* FTAAbility = Cast<UFTAGameplayAbility>(Spec->Ability))
					{
						UE_LOG(LogTemp, Warning, TEXT("Ability name: %s"), *FTAAbility->GetName());
						// InstanceData.AbilityAsset = FTAAbility->datass;
					}
				}
			}
		}
	}
}

void FStateTreeEvaluator_SelectBestAbility::TreeStop(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStop(Context);
}

void FStateTreeEvaluator_SelectBestAbility::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FStateTreeEvaluatorCommonBase::Tick(Context, DeltaTime);
}
