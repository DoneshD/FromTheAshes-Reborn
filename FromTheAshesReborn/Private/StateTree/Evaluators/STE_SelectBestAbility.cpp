#include "StateTree/Evaluators/STE_SelectBestAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_Attack.h"

void FStateTreeEvaluator_SelectBestAbility::TreeStart(FStateTreeExecutionContext& Context) const
{
	FStateTreeEvaluatorCommonBase::TreeStart(Context);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if(!InstanceData.OwningActor)
	{
		UE_LOG(LogTemp, Error, TEXT("OwningActor is null"))
		return;
	}

	TArray<TObjectPtr<UFTAGameplayAbility>> PossibleAbilities;
	
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
						if (FTAAbility->IsA(InstanceData.AbilityClass))
						{
							UE_LOG(LogTemp, Warning, TEXT("Ability added: %s"), *FTAAbility->GetName());
							PossibleAbilities.Add(FTAAbility);
						}
					}
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Num: %d"), PossibleAbilities.Num());
	if (PossibleAbilities.Num() > 0)
	{
		auto RandomItem = PossibleAbilities[FMath::RandRange(0, PossibleAbilities.Num() - 1)];
		UE_LOG(LogTemp, Warning, TEXT("Ability Chosen: %s"), *RandomItem->GetName());
		InstanceData.AbilityAsset = RandomItem->DefaultAbilityDataAsset;
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
