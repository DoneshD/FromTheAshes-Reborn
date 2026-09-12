#include "FTAAbilitySystem/GameplayAbilities/GA_Execution.h"

#include "ContextualAnimSceneActorComponent.h"
#include "ContextualAnimTypes.h"
#include "ContextualAnimUtilities.h"
#include "DefaultLevelSequenceInstanceData.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Camera/CameraSystemComponent.h"
#include "FTACustomBase/FTACharacter.h"

bool UGA_Execution::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Execution::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(!GetFTAPlayerStateFromOwnerInfo()->HardLockedTargetActor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	UCameraSystemComponent* CSC = GetFTACharacterFromActorInfo()->FindComponentByClass<UCameraSystemComponent>();
	
	if(!CSC)
	{
		UE_LOG(LogTemp, Error, TEXT("No CSC executions"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	CSC->ToggleCameraSystem(false);

	if(!PossibleExecutions[0].ContextScene || PossibleExecutions.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("No Possible executions"));
		
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	FContextualAnimSceneBindingContext PrimaryContextScene(GetFTACharacterFromActorInfo());
	FContextualAnimSceneBindingContext SecondaryContextScene(GetFTAPlayerStateFromOwnerInfo()->HardLockedTargetActor);

	FContextualAnimSceneBindings OutBinding;
	
	UContextualAnimUtilities::BP_CreateContextualAnimSceneBindingsForTwoActors(PossibleExecutions[0].ContextScene, PrimaryContextScene, SecondaryContextScene, OutBinding);
	
	GetFTACharacterFromActorInfo()->ContextualAnimSceneActorComponent->StartContextualAnimScene(OutBinding);

	GetFTACharacterFromActorInfo()->ContextualAnimSceneActorComponent->OnLeftSceneDelegate.AddDynamic(this, &UGA_Execution::OnLeftScene);

	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	ALevelSequenceActor* SequenceActor;
	
	 ULevelSequencePlayer* LevelSequence = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(),
	 	PossibleExecutions[0].LevelSequence, PlaybackSettings, SequenceActor);

	if(!LevelSequence)
	{
		UE_LOG(LogTemp, Error, TEXT("Level sequence playback failed"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	SequenceActor->bOverrideInstanceData = true;
	UDefaultLevelSequenceInstanceData* InstanceData = Cast<UDefaultLevelSequenceInstanceData>(SequenceActor->DefaultInstanceData);
	
	if(!InstanceData)
	{
		UE_LOG(LogTemp, Error, TEXT("Level sequence InstanceData failed"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	InstanceData->TransformOriginActor = GetFTACharacterFromActorInfo();
	UE_LOG(LogTemp, Warning, TEXT("Playing"));
	LevelSequence->Play();
}

void UGA_Execution::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_Execution::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (GetFTACharacterFromActorInfo()->ContextualAnimSceneActorComponent)
	{
		GetFTACharacterFromActorInfo()->ContextualAnimSceneActorComponent->OnLeftSceneDelegate.RemoveDynamic(this, &UGA_Execution::OnLeftScene);
	}

}

void UGA_Execution::OnLeftScene(UContextualAnimSceneActorComponent* SceneActorComp)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
