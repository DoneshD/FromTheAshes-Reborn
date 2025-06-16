#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingSystemComponent.generated.h"

class AFTAPlayerCameraManger;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float MinimumDistanceToEnable = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	TSubclassOf<AActor> TargetableActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	TEnumAsByte<ECollisionChannel> TargetableCollisionChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool ShouldControlRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool IgnoreLookInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float BreakLineOfSightDelay = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float StartRotatingThreshold = 0.85f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	bool ShouldDrawLockedOnWidget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> LockedOnWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	float LockedOnWidgetDrawSize = 32.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FName LockedOnWidgetParentSocket = FName("spine_03");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	FVector LockedOnWidgetRelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Offset | Distance")
	float MaxDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Offset | Distance")
	float MinDistance = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Offset | Pitch Offset")
	bool ShouldAdjustPitchBasedOnDistanceToTarget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Offset | Pitch Offset")
	float MaxPitchOffset = -20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Offset | Yaw Offset")
	bool ShouldAdjustYawBasedOnDistanceToTarget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Offset | Yaw Offset")
	float MaxYawOffset = -35.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticky Switch")
	bool EnableStickyTarget = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticky Switch")
	float AxisMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticky Switch")
	float StickyRotationThreshold = 30.0f;

	UPROPERTY(BlueprintAssignable, Category = "Target System Delegates")
	FTraceComponentOnTargetLockedOnOff OnTargetLockedOff;

	UPROPERTY(BlueprintAssignable, Category = "Target System Delegates")
	FTraceComponentOnTargetLockedOnOff OnTargetLockedOn;
	
	UPROPERTY(BlueprintAssignable, Category = "Target System Delegates")
	FTraceComponentSetRotation OnTargetSetRotation;

public:
	UPROPERTY(EditAnywhere, Category = "Rotation Offset | Control Offset")
	bool EnableSoftLockCameraOffset = true;

	UPROPERTY(EditAnywhere, Category = "Rotation Offset | Control Offset")
	float SoftLockDecayRate = 0.75f;

	UPROPERTY(EditAnywhere, Category = "Rotation Offset | Control Offset")
	float CameraInputScale = 1.5f;
	
	UPROPERTY(EditAnywhere, Category = "Control Offset | Yaw Offset")
	float MaxSoftYawOffset = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Control Offset | Pitch Offset")
	float MaxSoftPitchOffset = 10.0f;

	float CatchupInterpSpeed = 8.0f;
	
	FRotator CurrentCameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	bool ShouldUpdateControllerRotation = false;

private:
	UPROPERTY()
	AActor* OwnerActor;

	UPROPERTY()
	APawn* OwnerPawn;

	UPROPERTY()
	APlayerController* OwnerPlayerController;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<AFTAPlayerCameraManger> FTAPlayerCameraManger;


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
	
	FVector SmoothedMidPoint = FVector::ZeroVector;

protected:

	UTargetingSystemComponent();
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DrawCameraAnchor();

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
	float CatchupToOffScreen(const FVector& PlayerLocation, float& InInterpSpeed);
	void UpdateTargetingCameraAnchorAndRotation(APlayerCharacter* PlayerOwner, const AActor* TargetActor);
	void ControlRotation(bool ShouldControlRotation) const;

	float GetAngleUsingCameraRotation(const AActor* ActorToLook) const;
	float GetAngleUsingCharacterRotation(const AActor* ActorToLook) const;

	static FRotator FindLookAtRotation(const FVector Start, const FVector Target);

	float GetWorldDistanceFromCamera(APlayerController* PlayerController, const AActor* TargetActor);
	float CompareDistanceToScreenAndGetInterpSpeed(APlayerCharacter* PlayerOwner, const AActor* TargetActor, bool& InShouldUpdateControlRotation);
	
	void DisableMidPointControlRotation();
	
	void ControlCameraOffset(float DeltaTime);
	
	void CreateAndAttachTargetLockedOnWidgetComponent(AActor* TargetActor);
	
	void TargetLockOn(AActor* TargetToLockOn);
	void ResetIsSwitchingTarget();
	bool ShouldSwitchTargetActor(float AxisValue);

	static bool TargetIsTargetable(const AActor* Actor);
	
	void SetupLocalPlayerController();

public:

	UFUNCTION(BlueprintCallable, Category = "Target System")
	AActor* GetLockedOnTargetActor() const;

	UFUNCTION(BlueprintCallable, Category = "Target System")
	bool IsLocked() const;

	//GA_LockOn entry point
	UFUNCTION(BlueprintCallable, Category = "Target System")
	AActor* TargetActor(bool& IsSuccess);

	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetLockOff();
	
	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetActorWithAxisInput(float AxisValue);

	UFUNCTION(BlueprintCallable, Category = "Target System")
	bool GetTargetLockedStatus();
	
};
