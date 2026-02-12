#include "FTAMotionWarpingComponent.h"

#include "MotionWarpingComponent.h"
#include "CombatComponents/AfterImageComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FTAPlayerState.h"

UFTAMotionWarpingComponent::UFTAMotionWarpingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UFTAMotionWarpingComponent::BeginPlay()
{
	Super::BeginPlay();

	FTACharacter = Cast<AFTACharacter>(GetOwner());

	if (!FTACharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UMeleeWarpingComponent::BeginPlay - FTACharacter is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}

	MotionWarpingComponent= FTACharacter->FindComponentByClass<UMotionWarpingComponent>();

	if (!MotionWarpingComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UMeleeWarpingComponent::BeginPlay - MotionWarpingComp is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}
	
}

void UFTAMotionWarpingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UFTAMotionWarpingComponent::UpdateWarpTarget(FVector TargetLocation, FRotator TargetRotation)
{
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(CurrentWarpTargetName, TargetLocation,  FRotator(0, TargetRotation.Yaw, 0));
}

void UFTAMotionWarpingComponent::WarpToTarget(FWarpData WarpData)
{
	CurrentWarpTargetName = WarpData.WarpTargetName;
	
	//Actor warp
	if(WarpData.WarpTargetActor)
	{
		AEnemyBaseCharacter* EnemyActor = Cast<AEnemyBaseCharacter>(WarpData.WarpTargetActor);
		if (EnemyActor && !EnemyActor->IsDead)
		{
			FVector EnemyLocation = EnemyActor->GetActorLocation();
			FVector OffsetDirection = (GetOwner()->GetActorLocation() - EnemyActor->GetActorLocation()).GetSafeNormal();
			
			OffsetDirection.Z = 0.0f;
			
			FVector WarpTargetLocation = EnemyLocation + OffsetDirection * WarpData.WarpTargetLocationOffset;
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), EnemyActor->GetActorLocation());
			FRotator WarpTargetRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

			// DrawDebugSphere(GetWorld(), WarpTargetLocation, 25.0f, 12, FColor::Blue, false, 5.0f);
		
			// const FVector Forward = WarpTargetRotation.Vector();
			// DrawDebugLine(
			// 	GetWorld(),
			// 	WarpTargetLocation,
			// 	WarpTargetLocation + Forward * 100.0f,
			// 	FColor::Cyan,
			// 	false,
			// 	5.0f,
			// 	0,
			// 	2.0f
			// );

			// DrawDebugSphere(GetWorld(), ClosestActorToOwner->GetActorLocation(), 25.0f, 12, FColor::Blue, false, 5.0f);
			// DrawDebugSphere(GetWorld(), ClosestActorToAxis->GetActorLocation(), 25.0f, 12, FColor::Red, false, 5.0f);
			
			UAfterImageComponent* AIC = FTACharacter->FindComponentByClass<UAfterImageComponent>();
			
			if(GetOwner()->GetDistanceTo(EnemyActor) >= 350.0f)
			{
				AIC->CreateAfterImage();
			}
		
			UpdateWarpTarget(WarpTargetLocation, WarpTargetRotation);
		}
	}
	
	
}

void UFTAMotionWarpingComponent::RemoveWarpTarget()
{
	MotionWarpingComponent->RemoveWarpTarget(CurrentWarpTargetName);
}
