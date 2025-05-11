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
