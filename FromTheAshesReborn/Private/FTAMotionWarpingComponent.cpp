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

	PlayerState = Cast<AFTAPlayerState>(FTACharacter->GetPlayerState());
	if(!PlayerState)
	{
		// UE_LOG(LogTemp, Error, TEXT("[%s] UMeleeWarpingComponent::BeginPlay - PlayerState is null"), *GetOwner()->GetActorNameOrLabel());
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

void UFTAMotionWarpingComponent::RemoveWarpTarget()
{
	MotionWarpingComponent->RemoveWarpTarget(CurrentWarpTargetName);
}

void UFTAMotionWarpingComponent::WarpToTarget(FMeleeWarpData WarpData, bool InvertLocation, bool TestHitWarp)
{
	CurrentWarpTargetName = WarpData.WarpTargetName;

	if(TestHitWarp)
	{
		FVector TestLocation;

		TestLocation = GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * -WarpData.StartTraceLocationOffset);

		DrawDebugSphere(
			GetWorld(),
			TestLocation,
			25.f,       
			12,          
			FColor::Red, 
			false,       
			2.f          
		);
		
		UpdateWarpTarget(TestLocation, GetOwner()->GetActorRotation());
		return;
	}
	
}