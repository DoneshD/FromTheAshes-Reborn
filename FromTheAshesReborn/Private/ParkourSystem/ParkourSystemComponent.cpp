#include "ParkourSystem/ParkourSystemComponent.h"
#include "GameplayTagsManager.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ParkourSystem/ParkourFunctionLibrary.h"
#include "ParkourSystem/ArrowActor.h"
#include "Camera/CameraComponent.h"
#include "ParkourSystem/ParkourVariableDataAsset.h"

void UParkourSystemComponent::DrawDebugRotationLines(FRotator InRotation)
{
	// Calculate direction vectors
	FVector ForwardVector = InRotation.Vector();  // This is equivalent to `GetForwardVector()`
	FVector RightVector = FRotationMatrix(InRotation).GetScaledAxis(EAxis::Y);  // Y axis
	FVector UpVector = FRotationMatrix(InRotation).GetScaledAxis(EAxis::Z);  // Z axis

	// Define line colors
	FColor ForwardColor = FColor::Red;
	FColor RightColor = FColor::Green;
	FColor UpColor = FColor::Blue;

	// Draw the lines
	DrawDebugLine(GetWorld(), OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + (ForwardVector * 100), ForwardColor, false, 5, 0, 1);
	DrawDebugLine(GetWorld(), OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + (RightVector * 100), RightColor, false, 5, 0, 1);
	DrawDebugLine(GetWorld(), OwnerCharacter->GetActorLocation(), OwnerCharacter->GetActorLocation() + (UpVector * 100), UpColor, false, 5, 0, 1);
}

UParkourSystemComponent::UParkourSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;


}

void UParkourSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	
	if(!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerCharacter is NULL"));
		return;
	}
	
	CharacterMovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>());
	
	if(!CharacterMovementComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterMovementComponent is NULL"));
		return;
	}
}

void UParkourSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UParkourSystemComponent::ParkourInputPressedVault()
{
	SelectParkourAction(false);
}

bool UParkourSystemComponent::SetIntializeReference(ACharacter* Character, USpringArmComponent* CameraBoom,
	UCameraComponent* Camera, UMotionWarpingComponent* MotionWarping)
{
	
	OwnerCharacter = Character;
	
	if(!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerCharacter is NULL"));
		return false;
	}
	
	CharacterMovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->FindComponentByClass<UCharacterMovementComponent>());
	
	if(!CharacterMovementComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterMovementComponent is NULL"));
		return false;
	}

	SkeletalMeshComponent = Cast<USkeletalMeshComponent>(OwnerCharacter->FindComponentByClass<USkeletalMeshComponent>());

	if(!SkeletalMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("SkeletalMesh is NULL"));
		return false;
	}

	AnimInstance = SkeletalMeshComponent->GetAnimInstance();

	if(!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("AnimInstance is NULL"));
		return false;
	}

	CapsuleComponent = OwnerCharacter->GetCapsuleComponent();

	if(!CapsuleComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CapsuleComponent is NULL"));
		return false;
	}

	CameraBoomComponent = CameraBoom;

	if(!CameraBoomComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraBoomComponent is NULL"));
		return false;
	}

	CameraComponent = Camera;

	if(!CameraComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraComponent is NULL"));
		return false;
	}

	MotionWarpingComponent = MotionWarping;
	
	if(!MotionWarpingComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("MotionWarpingComponent is NULL"));
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;

	FTransform SpawnTransform(OwnerCharacter->GetActorTransform());
	
	FAttachmentTransformRules WidgetTransformRules(EAttachmentRule::SnapToTarget, true);
	
	FAttachmentTransformRules TransformRules(EAttachmentRule::KeepRelative, true);
	ArrowActor = GetWorld()->SpawnActor<AArrowActor>(ArrowActorClass, SpawnTransform, SpawnParams);

	if(!ArrowActor)
	{
		UE_LOG(LogTemp, Error, TEXT("ArrowActor is NULL"));
		return false;
	}
	ArrowActor->AttachToComponent(SkeletalMeshComponent, TransformRules);
	ArrowActor->SetActorRelativeLocation(FVector(ArrowLocationX, 0, ArrowLocationZ - CharacterHeightDiff));

	if(!CameraBoomComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraBoomComponent is NULL"));
		return false;
	}

	FirstTargetArmLength = CameraBoomComponent->TargetArmLength;
	FirstArmRelativeLocation = CameraBoomComponent->GetRelativeLocation();
	
	return false;
}

bool UParkourSystemComponent::SelectClimb(bool InCanManualClimb, bool InCanAutoClimb, bool InAutoClimb)
{
	return InAutoClimb ? InCanAutoClimb : InCanManualClimb;
}

void UParkourSystemComponent::SelectParkourAction(bool InAutoClimb)
{
	if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction")))
	{
		if(SelectClimb(CanManualClimb, CanAutoClimb, MemberAutoClimb))
		{
			ResetParkourResult();
			FindParkourLocationAndShape();
			ShowHitResults();
			FindSizeParkourObjects();
			FindParkourType(MemberAutoClimb);

		}
	}
}

void UParkourSystemComponent::FindParkourLocationAndShape()
{
	FVector CapsuleStartLocation;
	FVector CapsuleEndLocation;
	float CapsuleRadius;
	float CapsuleHalfHeight;
	const TArray<AActor*> ActorsToIgnore;
	FHitResult InitialCapsuleTraceOutHitResult;
	
	GetInitialCapsuleTraceSettings(CapsuleStartLocation, CapsuleEndLocation, CapsuleRadius, CapsuleHalfHeight);
	bool bInitialCapsuleTraceOutHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), CapsuleStartLocation, CapsuleEndLocation, CapsuleRadius, CapsuleHalfHeight, TraceTypeQuery1, false,
		ActorsToIgnore, EDrawDebugTrace::None, InitialCapsuleTraceOutHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

	if(InitialCapsuleTraceOutHitResult.bBlockingHit)
	{
		WallHeightTraces.Empty();
		for(int32 i = 0; i <= HorizontalWallDetectTraceHalfQuantity * 2; i++)
		{
			PotentialWallHeightTraces.Empty();
			for(int32 j = 0; j <= VerticalWallDetectTraceQuantity/ (CharacterMovementComponent->IsFalling() ? 2 : 1); j++)
			{
				FVector Vector1 = FVector(i * HorizontalWallDetectTraceRange + HorizontalWallDetectTraceRange * HorizontalWallDetectTraceHalfQuantity * -1.0f);
				FRotator NormalizeCapusleRotation = UParkourFunctionLibrary::NormalReverseRotationZ(InitialCapsuleTraceOutHitResult.ImpactNormal);

				// DrawDebugRotationLines(NormalizeCapusleRotation);
				
				FVector Vector2 = Vector1 * UKismetMathLibrary::GetRightVector(NormalizeCapusleRotation);
				FVector InitialWallHitLocation = FVector(InitialCapsuleTraceOutHitResult.ImpactPoint.X, InitialCapsuleTraceOutHitResult.ImpactPoint.Y, GetVerticalWallDetectStartHeight());

				FVector WallTraceLocation = FVector(0.0f, 0.0f, j * VerticalWallDetectTraceRange) + Vector2 + InitialWallHitLocation;

				//HopHit Traces
				FVector StartWallHitTraceLocation = WallTraceLocation + (UKismetMathLibrary::GetForwardVector(NormalizeCapusleRotation) * -40.0f);
				FVector EndWallHitTraceLocation = WallTraceLocation + (UKismetMathLibrary::GetForwardVector(NormalizeCapusleRotation) * 30.0f);

				FHitResult PotentialWallHeightHitOutResult;
				bool bPotentialWallHeightHitOut =  GetWorld()->LineTraceSingleByChannel(PotentialWallHeightHitOutResult, StartWallHitTraceLocation, EndWallHitTraceLocation, ECC_Visibility);

				// if (bPotentialWallHeightHitOut)
				// {
				// 	DrawDebugLine(GetWorld(), StartWallHitTraceLocation, PotentialWallHeightHitOutResult.Location, FColor::Green, false, 1.0f, 0, 2.0f);
					// DrawDebugPoint(GetWorld(), PotentialWallHeightHitOutResult.Location, 10.0f, FColor::Red, false, 1.0f);
				// }
				// else
				// {
				// 	DrawDebugLine(GetWorld(), StartWallHitTraceLocation, EndWallHitTraceLocation, FColor::Red, false, 1.0f, 0, 2.0f);
				// }

				PotentialWallHeightTraces.Add(PotentialWallHeightHitOutResult);
				
			}
			int32 k = 0;
			for (FHitResult PotentialHeightHit : PotentialWallHeightTraces)
			{
				if(k != 0)
				{
					float CurrentTraceDistance = FVector::Dist(PotentialHeightHit.TraceStart, PotentialHeightHit.TraceEnd);
					float SelectCurrentTraceDistance = PotentialHeightHit.bBlockingHit ? PotentialHeightHit.Distance : CurrentTraceDistance;

					float PrevouisTraceDistance = FVector::Dist(PotentialWallHeightTraces[k - 1].TraceStart, PotentialWallHeightTraces[k - 1].TraceEnd);
					float SelectPrevouisTraceDistance = PotentialWallHeightTraces[k - 1].bBlockingHit ? PotentialWallHeightTraces[k - 1].Distance : PrevouisTraceDistance;

					if(SelectCurrentTraceDistance - SelectPrevouisTraceDistance > 5.0f)
					{
						WallHeightTraces.Add(PotentialWallHeightTraces[k - 1]);
					}
				}
				k++;
			}
		}
		
		int32 l = 0;
		for (FHitResult OutWallHitResult : WallHeightTraces)
		{
			if(l == 0)
			{
				WallHitResult = OutWallHitResult;
			}
			else
			{
				if(FVector::Dist(OutWallHitResult.ImpactPoint, OwnerCharacter->GetActorLocation()) <= FVector::Dist(WallHitResult.ImpactPoint, OwnerCharacter->GetActorLocation()))
				{
					WallHitResult = OutWallHitResult;
				}
				else
				{
					// WallHitResult = WallHitResult;
				}
			}
			l++;
		}
		
		if(WallHitResult.bBlockingHit && !WallHitResult.bStartPenetrating)
		{
			if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Climb")))
			{
				
			}
			else
			{
				WallRotation = UParkourFunctionLibrary::NormalReverseRotationZ(WallHitResult.ImpactNormal);
			}
			for(int32 m = 0; m <= ForwardDistanceTraceQuantity; m++)
			{
				FVector StartingPositionOffset = UKismetMathLibrary::GetForwardVector(WallRotation) * 10.0f;
				FVector ForwardDistance = UKismetMathLibrary::GetForwardVector(WallRotation) * (m * ForwardDistanceTraceBetweenSpace);
				FVector ImpactPointForwardVector = WallHitResult.ImpactPoint + ForwardDistance + StartingPositionOffset;
				
				FVector SphereTraceStartLocation = ImpactPointForwardVector + FVector(0.0f, 0.0f, 7.0f);
				FVector SphereTraceEndLocation = SphereTraceStartLocation - FVector(0.0f, 0.0f, 7.0f);

				FHitResult FWallDepthOutHitResult;
				TArray<AActor*> ActorsToIgnoreSphere;

				bool bWallDepthTraceOutHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), SphereTraceStartLocation, SphereTraceEndLocation, 10.5f, TraceTypeQuery1,
					false, ActorsToIgnore, EDrawDebugTrace::None, FWallDepthOutHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

				if(m == 0)
				{
					if(bWallDepthTraceOutHit)
					{
						WallTopResult = FWallDepthOutHitResult;
						WarpTopPoint = FWallDepthOutHitResult.ImpactPoint;
					}
				}
				
				if(bWallDepthTraceOutHit)
				{
					WallTopHit = FWallDepthOutHitResult;
					if(m == ForwardDistanceTraceQuantity)
					{
						FindWallDepth();
					}
				}
				else
				{
					break;
				}
			}
			FindWallDepth();
		}
	}
}

void UParkourSystemComponent::FindSizeParkourObjects()
{
	if(WallHitResult.bBlockingHit)
	{
		if(WallTopResult.bBlockingHit)
		{
			if(OwnerCharacter->GetMesh())
			{
				WallHeight = WallTopResult.ImpactPoint.Z - OwnerCharacter->GetMesh()->GetSocketLocation(FName("root")).Z;
			}
		}
		else
		{
			WallHeight = 0.0f;
		}

		if(WallTopResult.bBlockingHit && WallDepthResult.bBlockingHit)
		{
			WallDepth = FVector::Dist(WallTopResult.ImpactPoint, WallDepthResult.ImpactPoint);
		}
		else
		{
			WallDepth = 0.0f;
		}

		if(WallDepthResult.bBlockingHit && WallVaultResult.bBlockingHit)
		{
			//Other side
			VaultHeight = WallDepthResult.ImpactPoint.Z - WallVaultResult.ImpactPoint.Z;
		}
		else
		{
			VaultHeight = 999.0f;
		}

		if(WallTopResult.bBlockingHit)
		{
			DistanceFromLedgeXY = FVector::Dist(OwnerCharacter->GetActorLocation(), WallTopResult.ImpactPoint);
		}
		else
		{
			DistanceFromLedgeXY = 9999.0f;
		}
	}
	else
	{
		WallHeight = 0.0f;
		WallDepth = 0.0f;
		VaultHeight = 0.0f;
		DistanceFromLedgeXY = 9999.0f;
	}
}

void UParkourSystemComponent::FindParkourType(bool InAutoClimb)
{
	if(WallTopResult.bBlockingHit)
	{
		if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy")))
		{
			FindVaultOrMantleType();
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Setting Parkour Action: NoAction (Wall not blocking)"));
		SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
		if(!InAutoClimb)
		{
			OwnerCharacter->Jump();
		}
	}
}


void UParkourSystemComponent::PlayParkourMontage()
{
	//TODO: Change later
	CurrentParkourVariables->ParkourMontage = CurrentParkourMontage;
	SetParkourState(FGameplayTag::RequestGameplayTag("Parkour.State.Vault"));
	
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Parkour1"), FindWarp1Location(-70, -60), WallRotation);
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Parkour2"), FindWarp2Location(-30, -60), WallRotation);
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("Parkour3"), FindWarp3Location(0, 0), WallRotation);
	AnimInstance->Montage_Play(CurrentParkourVariables->ParkourMontage);
	
	// Bind the OnMontageBlendingOut delegate to a custom function
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	BlendingOutDelegate.BindUObject(this, &UParkourSystemComponent::OnMontageBlendingOut);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, CurrentParkourVariables->ParkourMontage);
}

// Define the callback function for OnBlendOut
void UParkourSystemComponent::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	SetParkourState(FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy"));
	SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
}

void UParkourSystemComponent::SetParkourAction(FGameplayTag InNewParkourAction)
{
	if(!InNewParkourAction.MatchesTag(ParkourActionTag))
	{
		ParkourActionTag = InNewParkourAction;
		//IParkourABP_Interface* ParkourABP_Interface = Cast<IParkourABP_Interface>(AnimInstance);
		//ParkourABP_Interface->SetParkourAction(InNewParkourAction)
	
		if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction")))
		{
			CurrentParkourVariables = nullptr;
			ResetParkourResult();

		}
		else if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.HighLongVault")))
		{
			CurrentParkourVariables = ParkourVariablesArray[0];
		}
		
		else if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.HighAverageVault")))
		{
			CurrentParkourVariables = ParkourVariablesArray[1];
		}

		else if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.HighShortVault")))
		{
			CurrentParkourVariables = ParkourVariablesArray[2];
		}

		else if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.AverageLongVault")))
		{
			CurrentParkourVariables = ParkourVariablesArray[3];
		}

		else if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.AverageAverageVault")))
		{
			CurrentParkourVariables = ParkourVariablesArray[4];
		}

		else if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.AverageShortVault")))
		{
			CurrentParkourVariables = ParkourVariablesArray[5];
		}

		else if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.LowLongVault")))
		{
			CurrentParkourVariables = ParkourVariablesArray[6];
		}

		else if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.LowAverageVault")))
		{
			CurrentParkourVariables = ParkourVariablesArray[7];
		}

		else if(ParkourActionTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Action.LowShortVault")))
		{
			CurrentParkourVariables = ParkourVariablesArray[8];
		}
		
		if(CurrentParkourVariables)
		{
			PlayParkourMontage();
		}
	}
}

void UParkourSystemComponent::GetInitialCapsuleTraceSettings(FVector& OutStart, FVector& OutEnd, float& OutRadius, float& OutHalfHeight)
{
	static FGameplayTag ClimbTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.Climb"));

	if(ParkourStateTag.MatchesTag(StateNotBusyTag))
	{
		
	}
	else if(ParkourStateTag.MatchesTag(ClimbTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Parkour state"));
	}

	FVector TempStart = OwnerCharacter->GetActorLocation() + FVector(
		0.0f, 0.0f, CharacterMovementComponent->IsFalling() ? 15.0f : 75.0f);
	
	FVector Velocity = CharacterMovementComponent->Velocity.GetSafeNormal2D(0.0001);
	float OutRangeA = CharacterMovementComponent->IsFalling() ? 25.0f : 50.0f;
	float OutRangeB = CharacterMovementComponent->IsFalling() ? 100.0f : 200.0f;

	//I dont get the reason for this
	float ClampedRange = UKismetMathLibrary::MapRangeClamped(Velocity.Length(), 0.0f, 500.0f, OutRangeA, OutRangeB);

	FVector TempEnd = TempStart + (FVector(Velocity.X, Velocity.Y, 0.0f) * InitialCapsuleTraceDistance);
	
	OutStart = TempStart;
	OutEnd = TempEnd;
	OutRadius = 40.0f;
	OutHalfHeight = CharacterMovementComponent->IsFalling() ? 70.0f : 120.0f;
}

float UParkourSystemComponent::GetVerticalWallDetectStartHeight()
{
	static FGameplayTag NotBusyTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.NotBusy"));
	static FGameplayTag ClimbTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.Climb"));

	if(ParkourStateTag.MatchesTag(NotBusyTag))
	{
		return OwnerCharacter->GetActorLocation().Z - (CharacterMovementComponent->IsFalling() ? 40.0f : 70.0f);
	}
	else if(ParkourStateTag.MatchesTag(ClimbTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("UParkourSystemComponent::GetVerticalWallDetectStartHeight should not be here"))
		return 0.0f;
		if(!ArrowActor)
		{
			UE_LOG(LogTemp, Error, TEXT("ArrowActor is NULL"))
			return 0.0f;
		}
	}
	return 0.0f;
}

void UParkourSystemComponent::FindWallDepth()
{
	if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Climb")) || ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy")))
	{
		FVector WallEdgeStartLocation = WallTopHit.ImpactPoint + (UKismetMathLibrary::GetForwardVector(WallRotation) * 30.0f);
		FVector WallEdgeEndLocation = WallTopHit.ImpactPoint;
		
		FHitResult FWallEdgeOutHitResult;
		TArray<AActor*> ActorsToIgnoreWallEdge;
		
		bool bWallEdgeTraceOutHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), WallEdgeStartLocation, WallEdgeEndLocation, 10.0f, TraceTypeQuery1,
					false, ActorsToIgnoreWallEdge, EDrawDebugTrace::None, FWallEdgeOutHitResult, true, FLinearColor::Red, FLinearColor::Green, 30.0f);

		if(bWallEdgeTraceOutHit)
		{
			WallDepthResult = FWallEdgeOutHitResult;
			WarpDepth = WallDepthResult.ImpactPoint;

			FVector WallGroundStartLocation = WallDepthResult.ImpactPoint + (UKismetMathLibrary::GetForwardVector(WallRotation) * FindForwardGroundDistanceTrace);
			FVector WallGroundEndLocation = WallGroundStartLocation - FVector(0.0f, 0.0f, 400.0f);
		
			FHitResult FWallGroundOutHitResult;
			TArray<AActor*> ActorsToIgnoreWallGround;
		
			bool bWallGroundTraceOutHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), WallGroundStartLocation, WallGroundEndLocation, 30.0f, TraceTypeQuery1,
						false, ActorsToIgnoreWallGround, EDrawDebugTrace::None, FWallGroundOutHitResult, true, FLinearColor::Blue, FLinearColor::Yellow, 5.0f);

			if(bWallGroundTraceOutHit)
			{
				WallVaultResult = FWallGroundOutHitResult;
				WarpVaultLocation = WallVaultResult.ImpactPoint;
			}
		}
	}
}

bool UParkourSystemComponent::CheckMantleSurface()
{
	FHitResult CapsuleTraceOutHitResult;
	TArray<AActor*> ActorsToIgnore; 
	FVector CapsuleStartLocation = WarpTopPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetScaledCapsuleHalfHeight() + 8.0f);
	FVector CapsuleEndLocation = WarpTopPoint + FVector(0.0f, 0.0f, CapsuleComponent->GetScaledCapsuleHalfHeight() + 8.0f);

	bool bCapsuleTraceOutHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), CapsuleStartLocation, CapsuleEndLocation, 25.0f, CapsuleComponent->GetScaledCapsuleHalfHeight() - 8.0f, TraceTypeQuery1, false,
	ActorsToIgnore, EDrawDebugTrace::ForDuration, CapsuleTraceOutHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

	return !bCapsuleTraceOutHit;
}

bool UParkourSystemComponent::CheckVaultSurface()
{
	FHitResult CapsuleTraceOutHitResult;
	TArray<AActor*> ActorsToIgnore;
	
	FVector CapsuleStartLocation = WarpTopPoint + FVector(0.0f, 0.0f,(CapsuleComponent->GetScaledCapsuleHalfHeight() / 2) + 18.0f);
	FVector CapsuleEndLocation = WarpTopPoint + FVector(0.0f, 0.0f, (CapsuleComponent->GetScaledCapsuleHalfHeight() / 2) + 18.0f);
	
	float HalfHeight = (CapsuleComponent->GetScaledCapsuleHalfHeight() / 2) + 5.0f;

	bool bCapsuleTraceOutHit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), CapsuleStartLocation, CapsuleEndLocation, 25.0f, HalfHeight, TraceTypeQuery1, false,
	ActorsToIgnore, EDrawDebugTrace::ForDuration, CapsuleTraceOutHitResult, true, FLinearColor::Blue, FLinearColor::Yellow, 5.0f);

	return !bCapsuleTraceOutHit;
}

void UParkourSystemComponent::ShowHitResults()
{
	if(ShowHitResult)
	{
		if(WallTopHit.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), WallTopHit.ImpactPoint, 10.0f, 12, FColor::Green, false, 5.0f);
		}
		if(WallTopResult.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), WallTopResult.ImpactPoint, 10.0f, 12, FColor::Blue, false, 5.0f);
		}
		if(WallDepthResult.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), WallDepthResult.ImpactPoint, 10.0f, 12, FColor::Orange, false, 5.0f);
		}
		if(WallVaultResult.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), WallVaultResult.ImpactPoint, 10.0f, 12, FColor::Yellow, false, 5.0f);
		}
	}
}

void UParkourSystemComponent::SetParkourState(FGameplayTag InNewParkourState)
{
	if(!ParkourStateTag.MatchesTag(InNewParkourState))
	{
		ParkourStateTag = InNewParkourState;
		//IParkourABP_Interface* ParkourABP_Interface = Cast<IParkourABP_Interface>(AnimInstance);
		//ParkourABP_Interface->SetParkourState(InNewParkourAction)
		// IParkourStatsInterface* ParkourStatsInterface = Cast<IParkourStatsInterface>(WidgetActor);
		// if(ParkourStatsInterface)
		// {
		// 	ParkourStatsInterface->SetParkourState(FText::FromName(InNewParkourAction.GetTagName()));
		if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Climb")))
		{
			ParkourStateSettings(ECollisionEnabled::NoCollision, MOVE_Flying, FRotator(0.0f, 0.0f, 0.0f),true, true);
		}
		else if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Mantle")))
		{
			ParkourStateSettings(ECollisionEnabled::NoCollision, MOVE_Flying, FRotator(0.0f, 500.0f, 0.0f),true, false);
		}
		else if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.Vault")))
		{
			ParkourStateSettings(ECollisionEnabled::NoCollision, MOVE_Flying, FRotator(0.0f, 500.0f, 0.0f),true, false);
		}
		else if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.NotBusy")))
		{
			ParkourStateSettings(ECollisionEnabled::QueryAndPhysics, MOVE_Walking, FRotator(0.0f, 500.0f, 0.0f),true, false);
		}
		else if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.ReachLedge")))
		{
			ParkourStateSettings(ECollisionEnabled::NoCollision, MOVE_Flying, FRotator(0.0f, 500.0f, 0.0f),true, false);
		}
		else if(ParkourStateTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.State.LowMantle")))
		{
			
		}
	}
}

void UParkourSystemComponent::ParkourStateSettings(ECollisionEnabled::Type InCollisionType, EMovementMode InMode, FRotator InRotationRate, bool bDoCollisionTest, bool bStopMovementImmediately)
{
	CapsuleComponent->SetCollisionEnabled(InCollisionType);
	CharacterMovementComponent->SetMovementMode(InMode);
	CharacterMovementComponent->RotationRate = InRotationRate;
	CameraBoomComponent->bDoCollisionTest = bDoCollisionTest;
	if(bStopMovementImmediately)
	{
		CharacterMovementComponent->StopMovementImmediately();
	}
}

FVector UParkourSystemComponent::FindWarp1Location(float InWarp1XOffset, float InWarp1ZOffset)
{
	return WallTopResult.ImpactPoint + (UKismetMathLibrary::GetForwardVector(WallRotation) * InWarp1XOffset)  + FVector(0.0f, 0.0f, InWarp1ZOffset);
}

FVector UParkourSystemComponent::FindWarp2Location(float InWarp2XOffset, float InWarp2ZOffset)
{
	return WallDepthResult.ImpactPoint + (UKismetMathLibrary::GetForwardVector(WallRotation) * InWarp2XOffset) + FVector(0.0f, 0.0f, InWarp2ZOffset);
}

FVector UParkourSystemComponent::FindWarp3Location(float InWarp3XOffset, float InWarp3ZOffset)
{
	return WallVaultResult.ImpactPoint + (UKismetMathLibrary::GetForwardVector(WallRotation) * InWarp3XOffset) + FVector(0.0f, 0.0f, InWarp3ZOffset);
}

void UParkourSystemComponent::ResetParkourResult()
{
	PotentialWallHeightTraces.Empty();
	WallHeightTraces.Empty();
	WallHitResult.Reset(false, false);
	WallTopResult.Reset(false, false);
	WallTopHit.Reset(false, false);
	WallDepthResult.Reset(false, false);
	WallVaultResult.Reset(false, false);
}

void UParkourSystemComponent::FindVaultOrMantleType()
{
	if (WallHeight > 145)
	{
		CurrentVaultHeightTag = FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.High");
	}
	else if (WallHeight >= 60 && WallHeight <= 145)
	{
		CurrentVaultHeightTag = FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.Average");
	}
	else if (WallHeight < 60)
	{
		CurrentVaultHeightTag = FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.Low");
	}
	
	//Add a if condition to check if > 250 then mantle
	
	if (WallDepth > 200 && WallDepth <= 250)
	{
		CurrentVaultDepthTag = FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Long");
	}
	else if (WallDepth >= 60 && WallDepth <= 200)
	{
		CurrentVaultDepthTag = FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Average");
	}
	else if (WallDepth < 60)
	{
		CurrentVaultDepthTag = FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Short");
	}

	if(WallDepth == 0.0f || WallHeight == 0.0f)
	{
		SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
	}

	if(CheckVaultSurface())
	{
		//-------------Big,Big------
		if(CurrentVaultHeightTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.High")) && CurrentVaultDepthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Long")))
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.HighLongVault"));
		}

		//-------------Big,Average------
		else if(CurrentVaultHeightTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.High")) && CurrentVaultDepthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Average")))
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.HighAverageVault"));
		}

		//-------------Big,Small------
		else if(CurrentVaultHeightTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.High")) && CurrentVaultDepthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Short")))
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.HighShortVault"));
		}

		//-------------Average,Big------
		else if(CurrentVaultHeightTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.Average")) && CurrentVaultDepthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Long")))
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.AverageLongVault"));
		}

		//-------------Average,Average------
		else if(CurrentVaultHeightTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.Average")) && CurrentVaultDepthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Average")))
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.AverageAverageVault"));
		}

		//-------------Average,Small------
		else if(CurrentVaultHeightTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.Average")) && CurrentVaultDepthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Short")))
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.AverageShortVault"));
		}

		//-------------Low,Big------
		else if(CurrentVaultHeightTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.Low")) && CurrentVaultDepthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Long")))
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.LowLongVault"));
		}

		//-------------Low,Average------
		else if(CurrentVaultHeightTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.Low")) && CurrentVaultDepthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Average")))
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.LowAverageVault"));
		}

		//-------------Low,Small------
		else if(CurrentVaultHeightTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Height.Low")) && CurrentVaultDepthTag.MatchesTag(FGameplayTag::RequestGameplayTag("Parkour.Vault.Depth.Short")))
		{
			SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.LowShortVault"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Vault surfaced blocked"))
		SetParkourAction(FGameplayTag::RequestGameplayTag("Parkour.Action.NoAction"));
	}
}

void UParkourSystemComponent::PrintVaultHeightandDepth()
{
	UE_LOG(LogTemp, Warning, TEXT("WallHeight = %f"), WallHeight)
	UE_LOG(LogTemp, Warning, TEXT("WallDepth = %f"), WallDepth)
}
