﻿#include "FTAAbilitySystem/GameplayAbilities/Attack/GA_MeleeWeaponAttack_GroundPound.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "EventObjects/SlamEventObject.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AbilityTasks/AT_SlamCharacterAndWait.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperFunctionLibraries/LockOnFunctionLibrary.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_MeleeWeaponAttack_GroundPound::UGA_MeleeWeaponAttack_GroundPound(const FObjectInitializer&): TraceStartLocation(),
                                                                                                 TraceEndLocation(),
                                                                                                 GroundPoundEndLocation()
{
}

void UGA_MeleeWeaponAttack_GroundPound::OnAbilityTick(float DeltaTime)
{
	Super::OnAbilityTick(DeltaTime);

	if(IsGroundPounding)
	{
		TraceForActors();
	}
	
}

bool UGA_MeleeWeaponAttack_GroundPound::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	//Return true if owner is not player
	AFTAPlayerState* PS = Cast<AFTAPlayerState>(ActorInfo->OwnerActor.Get());
	if(ActorInfo->OwnerActor.Get() != PS)
	{
		return true;
	}
	
	if(!PS->HardLockedTargetActor)
	{
		return false;
	}
	
	float AngleOfInput = ULockOnFunctionLibrary::AngleFromInputVectorToLockedTarget(ActorInfo->AvatarActor.Get(), PS->HardLockedTargetActor);
	
	ELockOnInputOrientationDirection InputOrientation = ULockOnFunctionLibrary::GetOrientationOfInput(AngleOfInput);
	if(InputOrientation != ELockOnInputOrientationDirection::Backward)
	{
		return false;
	}
	
		

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_MeleeWeaponAttack_GroundPound::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//TODO: Need to change this
	GetFTAAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.EnableComponent")));
	
	FHitResult HitResult;
	TraceStartLocation = ActorInfo->AvatarActor.Get()->GetActorLocation();
	TraceEndLocation = TraceStartLocation + FVector(0.0f, 0.0f, -TraceVerticalDownwardDistance);

	FCollisionObjectQueryParams GroundCollisionQueryParams;
	GroundCollisionQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	GroundCollisionQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	

	if(bool bHitResult = GetWorld()->LineTraceSingleByObjectType(HitResult, ActorInfo->AvatarActor.Get()->GetActorLocation(), TraceEndLocation, GroundCollisionQueryParams))
	{
		GroundPoundEndLocation = HitResult.ImpactPoint;
		DrawDebugSphere(GetWorld(), GroundPoundEndLocation, 12, 12, FColor::Green, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Ground not found"));
	}
	
	DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Red, false, 2.0f, 0, 2.0f);


	GetFTACharacterFromActorInfo()->GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, 0.0f);
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 0.0;
	
	GetFTAAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("CombatMovementTag.Slam"));
	
	SlamTask = UAT_SlamCharacterAndWait::AT_SlamCharacterAndWait(
		this,
		GroundPoundEndLocation,
		GroundPoundSpeed,
		SlamDuration);
}

void UGA_MeleeWeaponAttack_GroundPound::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGA_MeleeWeaponAttack_GroundPound::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeWeaponAttack_GroundPound::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnMontageCancelled(EventTag, EventData);
}

void UGA_MeleeWeaponAttack_GroundPound::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
}

void UGA_MeleeWeaponAttack_GroundPound::EventMontageReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if(!UTagValidationFunctionLibrary::IsRegisteredGameplayTag(EventTag))
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack_Launcher::EventMontageReceived - EventTag is invalid"))
		return;
	}
	
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("CombatMovementTag.TestTag2")))
	{
		if (SlamTask)
		{
			SlamTask->OnSlamComplete.AddDynamic(this, &UGA_MeleeWeaponAttack_GroundPound::OnSlamComplete);
			SlamTask->ReadyForActivation();
		}

		IsGroundPounding = true;
		
	}
}

void UGA_MeleeWeaponAttack_GroundPound::SendMeleeHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	Super::SendMeleeHitGameplayEvents(TargetDataHandle);
	
}

void UGA_MeleeWeaponAttack_GroundPound::TraceForActors()
{
	//Testing sweep
		TArray<FHitResult> HitResults;
		TSet<AActor*> UniqueHitActors;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(GetFTACharacterFromActorInfo()->TargetObjectTraceChannel));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(GetFTACharacterFromActorInfo());

		FVector Start = GetFTACharacterFromActorInfo()->GetActorLocation();
		FVector End = Start + GetFTACharacterFromActorInfo()->GetActorForwardVector() * 250.0f;

		FVector HalfSize = FVector(100.0f, 100.0f, 100.0f); 
		FRotator Orientation = FRotator::ZeroRotator;

		UKismetSystemLibrary::BoxTraceMultiForObjects(
			GetWorld(),
			Start,
			End,
			HalfSize,
			Orientation,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration, 
			HitResults,
			true,                    
			FLinearColor::Red,
			FLinearColor::Green,
			2.0f
		);


		if(HitResults.Num() > 0 && HitResults.IsValidIndex(0))
		{
			for(FHitResult GPHitResult : HitResults)
			{
				if(GPHitResult.GetActor() && !UniqueHitActors.Contains(GPHitResult.GetActor()))
				{
					UniqueHitActors.Add(GPHitResult.GetActor());
					if(AEnemyBaseCharacter* EnemyBaseCharacter = Cast<AEnemyBaseCharacter>(GPHitResult.GetActor()))
					{
						FHitResult TempHitResult;
						FVector EnemyGroundPoundEndLocation;

						FCollisionObjectQueryParams GroundCollisionQueryParams;
						GroundCollisionQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
						GroundCollisionQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
						
						if(bool bHitResult = GetWorld()->LineTraceSingleByObjectType(TempHitResult, EnemyBaseCharacter->GetActorLocation(), TraceEndLocation, GroundCollisionQueryParams))
						{
							EnemyGroundPoundEndLocation = TempHitResult.ImpactPoint;
							DrawDebugSphere(GetWorld(), GroundPoundEndLocation, 12, 12, FColor::Green, true);
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Ground not found"));
						}
						SendHitGPEvent(GPHitResult, EnemyGroundPoundEndLocation, GroundPoundSpeed, SlamDuration);
					}
				}
			}
		}
}

void UGA_MeleeWeaponAttack_GroundPound::SendHitGPEvent(FHitResult HitItemToAdd, FVector LocationEnd, float Speed, float Duration)
{
	FGameplayAbilityTargetDataHandle TargetHitDataHandle = AddHitResultToTargetData(HitItemToAdd);
	
	USlamEventObject* SlamInfoObj = NewObject<USlamEventObject>(this);
	SlamInfoObj->SlamData.Location = LocationEnd;
	SlamInfoObj->SlamData.Speed = Speed;
	SlamInfoObj->SlamData.Duration = Duration;
	SlamInfoObj->HitData.Instigator = GetFTACharacterFromActorInfo();
	
	FGameplayEventData OnSlamHitEventData;
	OnSlamHitEventData.OptionalObject = SlamInfoObj;

	AActor* TargetActor = TargetHitDataHandle.Get(0)->GetHitResult()->GetActor();
	
	OnSlamHitEventData.Instigator = GetAvatarActorFromActorInfo();
	OnSlamHitEventData.Target = TargetActor;
	OnSlamHitEventData.ContextHandle.AddHitResult(*TargetHitDataHandle.Get(0)->GetHitResult());
	

	OnSlamHitEventData.EventTag = FGameplayTag::RequestGameplayTag("TestTag.Tag7");

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OnSlamHitEventData.EventTag, OnSlamHitEventData);
}

void UGA_MeleeWeaponAttack_GroundPound::OnSlamComplete()
{
	IsGroundPounding = false;
	GetFTAAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("CombatMovementTag.Slam"));
	PlayAbilityAnimMontage(SlamMontage);
	GetFTACharacterFromActorInfo()->GetCharacterMovement()->GravityScale = 4.0;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
