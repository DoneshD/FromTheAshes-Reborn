#include "AttacksComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DamageSystem/DamageSystem.h"
#include "Characters/EnemyMelee.h"
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
		EDrawDebugTrace::ForDuration,
		Hits,
		true);

	return bHit;
}

void UAttacksComponent::StartAttackCollisions()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Attack Collisions"));
	EmptyHitActorsArray();
	bActiveCollision = true;
}

void UAttacksComponent::EndAttackCollisions()
{
	bActiveCollision = false;

}

void UAttacksComponent::EmptyHitActorsArray()
{
	AlreadyHitActors_L.Empty();
	AlreadyHitActors_R.Empty();
}


void UAttacksComponent::MeleeTraceCollisions()
{
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
						20.0f,                            // DamageAmount
						EDamageType::EDamageType_Melee,   // DamageType
						EDamageResponse::EDamageResponse_None,  // DamageResponse
						false,                            // ShouldDamageInvincible
						false,                            // CanBeBlocked
						false,                            // CanBeParried
						false                             // ShouldForceInterrupt
					};

					DamagableInterface->NativeTakeDamage(DamageInfo);
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
							EDamageResponse::EDamageResponse_None,  // DamageResponse
							false,                            // ShouldDamageInvincible
							false,                            // CanBeBlocked
							false,                            // CanBeParried
							false                             // ShouldForceInterrupt
					};

					DamagableInterface->NativeTakeDamage(DamageInfo);
					
				}
			}
		}
	}
}

void UAttacksComponent::LightMeleeAttack(TObjectPtr<UAnimMontage> LightMeleeAttack)
{
	AActor* OwnerActor = GetOwner();
	
	if (OwnerActor)
	{
		AFTACharacter* FTACharacter = Cast<AFTACharacter>(OwnerActor);

		if (FTACharacter)
		{
			FTACharacter->PlayAnimMontage(LightMeleeAttack);
		}
	}
}