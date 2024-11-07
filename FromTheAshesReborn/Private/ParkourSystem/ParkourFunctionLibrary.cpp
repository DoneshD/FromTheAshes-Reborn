#include "ParkourSystem/ParkourFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

FRotator UParkourFunctionLibrary::NormalReverseRotationZ(FVector NormalVector)
{
	FRotator RotationFromX = UKismetMathLibrary::MakeRotFromX(NormalVector);
	FRotator NormalizeRotation = UKismetMathLibrary::NormalizedDeltaRotator(RotationFromX, FRotator(0, 180.0f, 0.0));
	
	return FRotator(0, NormalizeRotation.Yaw, 0);

}

FRotator UParkourFunctionLibrary::ReverseRotation(FRotator Rotation)
{
	return UKismetMathLibrary::NormalizedDeltaRotator(Rotation, FRotator(0, 180.0f, 0));
}

float UParkourFunctionLibrary::SelectClimbStyleFloat(float Braced, float FreeHang, FGameplayTag ClimbStyle)
{
	// Define tags for each climb style
	static FGameplayTag BracedTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.ClimbStyle.Braced"));
	static FGameplayTag FreeHangTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.ClimbStyle.FreeHang"));

	if (ClimbStyle.MatchesTag(BracedTag))
	{
		return Braced;
	}
	if (ClimbStyle.MatchesTag(FreeHangTag))
	{
		return FreeHang;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("No climb style tags match"));
	return 0.0f; 
}

float UParkourFunctionLibrary::SelectDirectionFloat(float Forward, float Backward, float Left, float Right,
	float ForwardLeft, float ForwardRight, float BackwardLeft, float BackwardRight, FGameplayTag Direction)
{
	static FGameplayTag ForwardTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.Forward"));
	static FGameplayTag BackwardTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.Backward"));
	static FGameplayTag LeftTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.Left"));
	static FGameplayTag RightTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.Right"));
	static FGameplayTag ForwardLeftTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.ForwardLeft"));
	static FGameplayTag ForwardRightTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.ForwardRight"));
	static FGameplayTag BackwardLeftTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.BackwardLeft"));
	static FGameplayTag BackwardRightTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.Direction.BackwardRight"));

	if (Direction.MatchesTag(ForwardTag))
	{
		return Forward;
	}
	else if (Direction.MatchesTag(BackwardTag))
	{
		return Backward;
	}
	else if (Direction.MatchesTag(LeftTag))
	{
		return Left;
	}
	else if (Direction.MatchesTag(RightTag))
	{
		return Right;
	}
	else if (Direction.MatchesTag(ForwardLeftTag))
	{
		return ForwardLeft;
	}
	else if (Direction.MatchesTag(ForwardRightTag))
	{
		return ForwardRight;
	}
	else if (Direction.MatchesTag(BackwardLeftTag))
	{
		return BackwardLeft;
	}
	else if (Direction.MatchesTag(BackwardRightTag))
	{
		return BackwardRight;
	}

	return 0.0f;
}

float UParkourFunctionLibrary::SelectParkourStateFloat(float NotBusy, float Vault, float Mantle, float Climb,
	FGameplayTag ParkourState)
{
	static FGameplayTag NotBusyTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.NotBusy"));
	static FGameplayTag VaultTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.Vault"));
	static FGameplayTag MantleTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.Mantle"));
	static FGameplayTag ClimbTag = FGameplayTag::RequestGameplayTag(TEXT("Parkour.State.Climb"));

	if (ParkourState.MatchesTag(NotBusyTag))
	{
		return NotBusy;
	}
	else if (ParkourState.MatchesTag(VaultTag))
	{
		return Vault;
	}
	else if (ParkourState.MatchesTag(MantleTag))
	{
		return Mantle;
	}
	else if (ParkourState.MatchesTag(ClimbTag))
	{
		return Climb;
	}

	return 0.0f;
}

FGameplayTag UParkourFunctionLibrary::SelectDirectionHopAction(FGameplayTag Forward, FGameplayTag Backward, FGameplayTag Left,
	FGameplayTag Right, FGameplayTag ForwardLeft, FGameplayTag ForwardRight, FGameplayTag BackwardLeft,
	FGameplayTag BackwardRight, FGameplayTag Direction)
{
	if (Direction.MatchesTag(Forward))
	{
		return Forward;
	}
	else if (Direction.MatchesTag(Backward))
	{
		return Backward;
	}
	else if (Direction.MatchesTag(Left))
	{
		return Left;
	}
	else if (Direction.MatchesTag(Right))
	{
		return Right;
	}
	else if (Direction.MatchesTag(ForwardLeft))
	{
		return ForwardLeft;
	}
	else if (Direction.MatchesTag(ForwardRight))
	{
		return ForwardRight;
	}
	else if (Direction.MatchesTag(BackwardLeft))
	{
		return BackwardLeft;
	}
	else if (Direction.MatchesTag(BackwardRight))
	{
		return BackwardRight;
	}

	return FGameplayTag();
}
