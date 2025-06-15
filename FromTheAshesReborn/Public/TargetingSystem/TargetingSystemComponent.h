#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingSystemComponent.generated.h"

class APlayerCharacter;
class UUserWidget;
class UWidgetComponent;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceComponentOnTargetLockedOnOff, AActor*, TargetActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTraceComponentSetRotation, AActor*, TargetActor, FRotator, ControlRotation);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROMTHEASHESREBORN_API UTargetingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Offset")
	bool EnableSoftLockCameraOffset = true;

	UPROPERTY(EditAnywhere, Category = "Offset")
	float MaxSoftYawOffset = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Offset")
	float MaxSoftPitchOffset = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Offset")
	float SoftLockDecayRate = 0.75f;

	UPROPERTY(EditAnywhere, Category = "Offset")
	float CameraInputScale = 1.5f;
	
	FRotator CurrentCameraOffset;


public:
	UTargetingSystemComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	float MinimumDistanceToEnable = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	TSubclassOf<AActor> TargetableActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	TEnumAsByte<ECollisionChannel> TargetableCollisionChannel;

	// Whether or not the character rotation should be controlled when Target is locked on.
	//
	// If true, it'll set the value of bUseControllerRotationYaw and bOrientationToMovement variables on Target locked on / off.
	//
	// Set it to true if you want the character to rotate around the locked on target to enable you to setup strafe animations.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	bool ShouldControlRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	bool IgnoreLookInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	float BreakLineOfSightDelay = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	float StartRotatingThreshold = 0.85f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
	bool ShouldDrawLockedOnWidget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
	TSubclassOf<UUserWidget> LockedOnWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
	float LockedOnWidgetDrawSize = 32.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
	FName LockedOnWidgetParentSocket = FName("spine_03");

	// The Relative Location to apply on Target LockedOn Widget when attached to a target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
	FVector LockedOnWidgetRelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Rotation Offset")
	float MaxDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Rotation Offset")
	float MinDistance = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Pitch Offset")
	bool ShouldAdjustPitchBasedOnDistanceToTarget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Pitch Offset")
	float MaxPitchOffset = -20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Yaw Offset")
	bool ShouldAdjustYawBasedOnDistanceToTarget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Yaw Offset")
	float MaxYawOffset = -35.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Sticky Feeling on Target Switch")
	bool EnableStickyTarget = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Sticky Feeling on Target Switch")
	float AxisMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Sticky Feeling on Target Switch")
	float StickyRotationThreshold = 30.0f;

	//GA_Lock entry point
	UFUNCTION(BlueprintCallable, Category = "Target System")
	AActor* TargetActor(bool& IsSuccess);

	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetLockOff();
	
	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetActorWithAxisInput(float AxisValue);

	UFUNCTION(BlueprintCallable, Category = "Target System")
	bool GetTargetLockedStatus();

	UPROPERTY(BlueprintAssignable, Category = "Target System")
	FTraceComponentOnTargetLockedOnOff OnTargetLockedOff;

	UPROPERTY(BlueprintAssignable, Category = "Target System")
	FTraceComponentOnTargetLockedOnOff OnTargetLockedOn;
	
	UPROPERTY(BlueprintAssignable, Category = "Target System")
	FTraceComponentSetRotation OnTargetSetRotation;

	UFUNCTION(BlueprintCallable, Category = "Target System")
	AActor* GetLockedOnTargetActor() const;

	UFUNCTION(BlueprintCallable, Category = "Target System")
	bool IsLocked() const;

private:
	UPROPERTY()
	AActor* OwnerActor;

	UPROPERTY()
	APawn* OwnerPawn;

	UPROPERTY()
	APlayerController* OwnerPlayerController;

	UPROPERTY()
	UWidgetComponent* TargetLockedOnWidgetComponent;

	UPROPERTY()
	AActor* LockedOnTargetActor;

	FTimerHandle LineOfSightBreakTimerHandle;
	FTimerHandle SwitchingTargetTimerHandle;

	bool IsBreakingLineOfSight = false;
	bool IsSwitchingTarget = false;
	bool IsTargetLocked = false;
	float ClosestTargetDistance = 0.0f;

	bool DesireToSwitch = false;
	float StartRotatingStack = 0.0f;
	

	TArray<AActor*> GetAllActorsOfClass(TSubclassOf<AActor> ActorClass) const;
	TArray<AActor*> FindTargetsInRange(TArray<AActor*> ActorsToLook, float RangeMin, float RangeMax) const;

	AActor* FindNearestTargetToActor(TArray<AActor*> Actors) const;
	AActor* FindNearestTargetToCenterViewport(TArray<AActor*> Actors) const;

	bool LineTrace(FHitResult& OutHitResult, const AActor* OtherActor, const TArray<AActor*>& ActorsToIgnore) const;
	bool LineTraceForActor(const AActor* OtherActor, const TArray<AActor*>& ActorsToIgnore) const;

	bool ShouldBreakLineOfSight() const;
	void BreakLineOfSight();

	bool IsInViewport(const AActor* TargetActor) const;
	float FindDistanceFromCenterOfViewport(const AActor* TargetActor) const;
	FVector2D FindCenterOfViewPort() const;

	float GetDistanceFromCharacter(const AActor* OtherActor) const;

	void SetOwnerActorRotation();

	float CalculateControlRotationOffset(float Distance, float MaxOffset) const;
	FRotator GetControlRotationOnTarget(const AActor* OtherActor) const;
	void SetControlRotationOnTarget(AActor* TargetActor) const;
	void ControlRotation(bool ShouldControlRotation) const;

	float GetAngleUsingCameraRotation(const AActor* ActorToLook) const;
	float GetAngleUsingCharacterRotation(const AActor* ActorToLook) const;

	static FRotator FindLookAtRotation(const FVector Start, const FVector Target);
	
	FVector CalculateMidpoint(FVector PlayerLocation, FVector TargetLocation);
	float CalculateDistance(FVector PlayerLocation, FVector TargetLocation);
	
	void UpdateMidPointControlRotation(APlayerCharacter* PlayerOwner, const AActor* TargetActor);
	void DisableMidPointControlRotation();
	
	void ControlCameraOffset(float DeltaTime);
	
	void CreateAndAttachTargetLockedOnWidgetComponent(AActor* TargetActor);
	
	void TargetLockOn(AActor* TargetToLockOn);
	void ResetIsSwitchingTarget();
	bool ShouldSwitchTargetActor(float AxisValue);

	static bool TargetIsTargetable(const AActor* Actor);
	
	 void SetupLocalPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DrawCameraAnchor();

	UPROPERTY(BlueprintReadWrite)
	bool IsTargeting = false;

	FVector MidPoint;
	float Radius;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;
	
	FVector SmoothedMidPoint = FVector::ZeroVector;

	
};
