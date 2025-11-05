#include "CombatComponents/DownedCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "CombatComponents/CentralStateComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/GameplayAbilities/Recover/GA_Recover.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"

UDownedCombatComponent::UDownedCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UDownedCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	FTACharacter = Cast<AFTACharacter>(GetOwner());

	if(!FTACharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UAerialCombatComponent::BeginPlay() - FTACharacter is null"));
		return;
	}

	FTAAbilitySystemComponent = FTACharacter->GetFTAAbilitySystemComponent();

	if(!FTAAbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UAerialCombatComponent::BeginPlay() - FTAAbilitySystemComponent is null"));
		return;
	}

	CMC = Cast<UCharacterMovementComponent>(FTACharacter->GetMovementComponent());

	if(!CMC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAerialCombatComponent::BeginPlay() - CMC is Null"));
		return;
	}

	FDelegateHandle Handle = FTAAbilitySystemComponent->RegisterGameplayTagEvent(EnableTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UDownedCombatComponent::EnableComponent);
	
}

void UDownedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(IsComponentActive)
	{
		TotalDownedTime += DeltaTime;
		if(TotalDownedTime >= 2.0f)
		{
			DisableComponent();
		}
	}

}

void UDownedCombatComponent::EnableComponent(const FGameplayTag InEnableTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		IsComponentActive = true;
		FTACharacter->CentralStateComponent->SetCurrentState(FGameplayTag::RequestGameplayTag("Character.State.Downed"));
	}
	else
	{
		
	}
}

void UDownedCombatComponent::DisableComponent()
{
	IsComponentActive = false;
	TotalDownedTime = 0.0f;
	FGameplayEventData OnRecoverEventData;
	

	//TODO: Native approach, fix later
	FTAAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("Character.State.Downed"));

	if(PossibleRecoveries[0] && PossibleRecoveries[0]->IsValidLowLevel() && PossibleRecoveries.Num() > 0 && !PossibleRecoveries.IsEmpty())
	{
		const UGA_Recover* const CDO = PossibleRecoveries[0]->GetDefaultObject<UGA_Recover>();
		if (CDO)
		{
			if(CDO->RecoverEffect)
			{
				
		
			}
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->RecoverTag))
			{
				OnRecoverEventData.EventTag = CDO->RecoverTag;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_Slammed::EndAbility - RecoveryTag is NULL"));
			}
		}
	}
			
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), OnRecoverEventData.EventTag, OnRecoverEventData);
}

