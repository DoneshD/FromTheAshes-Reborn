#include "WarpingManagerComponent.h"

#include "MotionWarpingComponent.h"
#include "Enemy/EnemyBaseCharacter.h"
#include "FTACustomBase/FTACharacter.h"
#include "HelperFunctionLibraries/InputReadingFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FTAPlayerState.h"

UWarpingManagerComponent::UWarpingManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UWarpingManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	FTACharacter = Cast<AFTACharacter>(GetOwner());

	if (!FTACharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UWarpingManagerComponent::BeginPlay - FTACharacter is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}

	MotionWarpingComponent= FTACharacter->FindComponentByClass<UMotionWarpingComponent>();

	if (!MotionWarpingComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UWarpingManagerComponent::BeginPlay - MotionWarpingComp is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}

	//TODO: WIll support AI use later
	PlayerState = Cast<AFTAPlayerState>(FTACharacter->GetPlayerState());
	if(!PlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] UWarpingManagerComponent::BeginPlay - PlayerState is null"), *GetOwner()->GetActorNameOrLabel());
		return;
	}
	
}

void UWarpingManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWarpingManagerComponent::AddWarpTarget(FVector TargetLocation, FRotator TargetRotation)
{
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(CurrentWarpTargetName, TargetLocation,  FRotator(0, TargetRotation.Yaw, 0));
}

void UWarpingManagerComponent::RemoveWarpTarget()
{
	MotionWarpingComponent->RemoveWarpTarget(CurrentWarpTargetName);
}

void UWarpingManagerComponent::TraceForTarget(FName WarpTargetName, float StartLocationOffset, float EndLocationDistance, float TraceRadius, float WarpTargetLocationOffset)
{
	CurrentWarpTargetName = WarpTargetName;
	
	FHitResult OutHit;
	FVector TraceStartLocation = GetOwner()->GetActorLocation() + GetTraceDirection() * StartLocationOffset;
	FVector TraceEndLocation =  GetOwner()->GetActorLocation() + GetTraceDirection() * EndLocationDistance;
	
	TArray<AActor*> ActorArray;
	ActorArray.Add(GetOwner());

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(FTACharacter->TargetObjectTraceChannel));
	
	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		TraceStartLocation,
		TraceEndLocation,
		TraceRadius,
		ObjectTypes,
		false,
		ActorArray,
		EDrawDebugTrace::None,
		OutHit,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		5.0f
	);
	

	//TODO: Change later
	AEnemyBaseCharacter* EnemyActor = Cast<AEnemyBaseCharacter>(OutHit.GetActor());
	if (bHit && EnemyActor && !EnemyActor->IsDead)
	{
		FVector OffsetDirection = (GetOwner()->GetActorLocation() - EnemyActor->GetActorLocation()).GetSafeNormal();
		
		FVector WarpTargetLocation = EnemyActor->GetActorLocation() + OffsetDirection * WarpTargetLocationOffset;
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), EnemyActor->GetActorLocation());
		FRotator WarpTargetRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

		// DrawDebugSphere(GetWorld(), WarpTargetLocation, 25.0f, 12, FColor::Blue, false, 5.0f);

		const FVector Forward = WarpTargetRotation.Vector();
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

		AddWarpTarget(WarpTargetLocation, WarpTargetRotation);
	}
}

FVector UWarpingManagerComponent::GetTraceDirection()
{
	FVector TraceDirection;

	//TODO: Error here, fix later
	if(!PlayerState->HardLockedTargetActor)
	{
		TraceDirection = UInputReadingFunctionLibrary::CheckInputVector(FTACharacter->GetCharacterMovement());
	}
	else
	{
		TraceDirection = (PlayerState->HardLockedTargetActor->GetActorLocation() - FTACharacter->GetActorLocation()).GetSafeNormal();
	}

	return TraceDirection;
}
	



