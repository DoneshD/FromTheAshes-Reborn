#include "CombatComponents/AttacksComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DamageSystem/DamageSystem.h"
#include "Characters/EnemyMelee.h"

#include "PlayMontageCallbackProxy.h"

#include "Interfaces/DamagableInterface.h"

UAttacksComponent::UAttacksComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAttacksComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAttacksComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bActiveCollision)
	{
		MeleeTraceCollisions();
	}
}

void UAttacksComponent::StartAttackCollisions()
{
	EmptyHitActorsArray();
	bActiveCollision = true;
}

void UAttacksComponent::EndAttackCollisions()
{
	bActiveCollision = false;
}

bool UAttacksComponent::MeleeWeaponSphereTrace(FVector StartLocation, FVector EndLocation, TArray<FHitResult>& Hits)
{
	TArray<AActor*> ActorArray;
	ActorArray.Add(GetOwner());

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		StartLocation,
		EndLocation,
		20.f,
		ObjectTypes,
		false,
		ActorArray,
		EDrawDebugTrace::None,
		Hits,
		true);

	return bHit;
}

void UAttacksComponent::EmptyHitActorsArray()
{
	AlreadyHitActors_L.Empty();
	AlreadyHitActors_R.Empty();
}

void UAttacksComponent::MeleeTraceCollisions()
{
	UE_LOG(LogTemp, Warning, TEXT("MeleeTraceCollisions(): SHOULD NOT BE HERE"));
	/*
	TArray<FHitResult> Hits;
	FVector StartLocation;
	FVector EndLocation;

	USkeletalMeshComponent* MeshComponent = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if (MeshComponent)
	{
		StartLocation = MeshComponent->GetSocketLocation("Start_L");
		EndLocation = MeshComponent->GetSocketLocation("End_L");
	}

	bool bLeftSuccess = MeleeWeaponSphereTrace(StartLocation, EndLocation, Hits);

	for (auto& CurrentHit : Hits)
	{
		AActor* HitActor = CurrentHit.GetActor();
		if (HitActor)
		{
			IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(HitActor);

			if (DamagableInterface)
			{
				if (!AlreadyHitActors_L.Contains(HitActor))
				{
					AlreadyHitActors_L.AddUnique(HitActor);
					FDamageInfo DamageInfo{
						20.0f,                               // DamageAmount
						EDamageType::EDamageType_Melee,      // DamageType
						EDamageResponse::EDamageResponse_HitReaction,  // DamageResponse
						false,                                // ShouldDamageInvincible
						false,                                // CanBeBlocked
						false,                                // CanBeParried
						false,                                // ShouldForceInterrupt
						HitReactionDirection        // HitReactionDirection
					};
					DamagableInterface->TakeDamage(DamageInfo);
				}
			}
		}
	}

	StartLocation = MeshComponent->GetSocketLocation("Start_R");
	EndLocation = MeshComponent->GetSocketLocation("End_R");

	bool bRightSuccess = MeleeWeaponSphereTrace(StartLocation, EndLocation, Hits);

	for (auto& CurrentHit : Hits)
	{
		AActor* HitActor = CurrentHit.GetActor();
		if (HitActor)
		{
			IDamagableInterface* DamagableInterface = Cast<IDamagableInterface>(HitActor);

			if (DamagableInterface)
			{

				if (!AlreadyHitActors_R.Contains(HitActor))
				{
					AlreadyHitActors_R.AddUnique(HitActor);
					FDamageInfo DamageInfo{
							20.0f,                            // DamageAmount
							EDamageType::EDamageType_Melee,   // DamageType
							EDamageResponse::EDamageResponse_HitReaction,  // DamageResponse
							false,                            // ShouldDamageInvincible
							false,                            // CanBeBlocked
							false,                            // CanBeParried
							false,                             // ShouldForceInterrupt
							HitReactionDirection
					};
					DamagableInterface->TakeDamage(DamageInfo);
				}
			}
		}
	}
	*/
}

void UAttacksComponent::LightMeleeAttack(TObjectPtr<UAnimMontage> LightMeleeAttack)
{
	AActor* OwnerActor = GetOwner();

	if (OwnerActor)
	{
		ACharacter* Character = Cast<ACharacter>(OwnerActor);

		if (Character)
		{
			Character->GetMesh()->GetAnimInstance()->Montage_Play(LightMeleeAttack);
			//FOnMontageEnded BlendOutDelegate;
			//BlendOutDelegate.BindUObject(this, &UAttacksComponent::FunctionToExecuteOnAnimationBlendOut);
			//Character->GetMesh()->GetAnimInstance()->Montage_SetBlendingOutDelegate(BlendOutDelegate, LightMeleeAttack);

			//FOnMontageEnded CompleteDelegate;
			//CompleteDelegate.BindUObject(this, &UAttacksComponent::FunctionToExecuteOnAnimationEnd);
			//Character->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(CompleteDelegate, LightMeleeAttack);
		}
	}
}

void UAttacksComponent::FinishLightMeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Incorrect FinishLightMeleeAttack"));
}

void UAttacksComponent::SetHitDirection(EHitReactionDirection Direction)
{
	HitReactionDirection = Direction;
}

void UAttacksComponent::FunctionToExecuteOnAnimationBlendOut(UAnimMontage* animMontage, bool bInterrupted)
{
	if (bInterrupted)
	{
		UE_LOG(LogTemp, Warning, TEXT("MY ANIMATION HAS INTERRUPTED!"));
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("MY ANIMATION IS BLENDING OUT!"));
	}
}

void UAttacksComponent::FunctionToExecuteOnAnimationEnd(UAnimMontage* animMontage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("MY ANIMATION HAS COMPLETED!"));
}
