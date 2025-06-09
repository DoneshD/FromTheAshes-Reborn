#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Launch/Resources/Version.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
#include "Engine/HitResult.h"
#else
#include "Engine/EngineTypes.h"
#endif
#include "TargetingSystemComponent.generated.h"

class APlayerCharacter;
class UUserWidget;
class UWidgetComponent;
class APlayerController;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentOnTargetLockedOnOff, AActor*, TargetActor);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FComponentSetRotation, AActor*, TargetActor, FRotator, ControlRotation);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTraceComponentOnTargetLockedOnOff, AActor*, TargetActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTraceComponentSetRotation, AActor*, TargetActor, FRotator, ControlRotation);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
	class FROMTHEASHESREBORN_API UTargetingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTargetingSystemComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	float MinimumDistanceToEnable = 4000.0f;

	// The AActor Subclass to search for targetable Actors.
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
	bool bShouldControlRotation = false;

	// Whether to accept pitch input when bAdjustPitchBasedOnDistanceToTarget is disabled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	bool bIgnoreLookInput = true;

	// The amount of time to break line of sight when actor gets behind an Object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	float BreakLineOfSightDelay = 2.0f;

	// Lower this value is, easier it will be to switch new target on right or left. Must be < 1.0f if controlling with gamepad stick
	//
	// When using Sticky Feeling feature, it has no effect (see StickyRotationThreshold)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System")
	float StartRotatingThreshold = 0.85f;

	// Whether or not the Target LockOn Widget indicator should be drawn and attached automatically.
	//
	// When set to false, this allow you to manually draw the widget for further control on where you'd like it to appear.
	//
	// OnTargetLockedOn and OnTargetLockedOff events can be used for this.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
	bool bShouldDrawLockedOnWidget = true;

	// The Widget Class to use when locked on Target. If not defined, will fallback to a Text-rendered
	// widget with a single O character.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
	TSubclassOf<UUserWidget> LockedOnWidgetClass;

	// The Widget Draw Size for the Widget class to use when locked on Target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Widget")
	float LockedOnWidgetDrawSize = 32.0f;

	// The Socket name to attach the LockedOn Widget.
	//
	// You should use this to configure the Bone or Socket name the widget should be attached to, and allow
	// the widget to move with target character's animation (Ex: spine_03)
	//
	// Set it to None to attach the Widget Component to the Root Component instead of the Mesh.
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
	bool bAdjustPitchBasedOnDistanceToTarget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Pitch Offset")
	float MaxPitchOffset = -20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Yaw Offset")
	bool bAdjustYawBasedOnDistanceToTarget = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Yaw Offset")
	float MaxYawOffset = -35.0f;
	

	// Set it to true / false whether you want a sticky feeling when switching target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Sticky Feeling on Target Switch")
	bool bEnableStickyTarget = false;

	// This value gets multiplied to the AxisValue to check against StickyRotationThreshold.
	//
	// Only used when Sticky Target is enabled.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Sticky Feeling on Target Switch")
	float AxisMultiplier = 1.0f;

	// Lower this value is, easier it will be to switch new target on right or left.
	//
	// This is similar to StartRotatingThreshold, but you should set this to a much higher value.
	//
	// Only used when Sticky Target is enabled.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target System|Sticky Feeling on Target Switch")
	float StickyRotationThreshold = 30.0f;

	// Function to call to target a new actor.
	UFUNCTION(BlueprintCallable, Category = "Target System")
	AActor* TargetActor(bool& bSuccess);

	// Function to call to manually untarget.
	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetLockOff();

	/**
	* Function to call to switch with X-Axis mouse / controller stick movement.
	*
	* @param AxisValue Pass in the float value of your Input Axis
	*/
	UFUNCTION(BlueprintCallable, Category = "Target System")
	void TargetActorWithAxisInput(float AxisValue);

	// Function to get TargetLocked private variable status
	UFUNCTION(BlueprintCallable, Category = "Target System")
	bool GetTargetLockedStatus();

	// Called when a target is locked off, either if it is out of reach (based on MinimumDistanceToEnable) or behind an Object.
	UPROPERTY(BlueprintAssignable, Category = "Target System")
	FTraceComponentOnTargetLockedOnOff OnTargetLockedOff;

	// Called when a target is locked on
	UPROPERTY(BlueprintAssignable, Category = "Target System")
	FTraceComponentOnTargetLockedOnOff OnTargetLockedOn;

	// Setup the control rotation on Tick when a target is locked on.
	//
	// If not implemented, will fallback to default implementation.
	// If this event is implemented, it lets you control the rotation of the character.
	UPROPERTY(BlueprintAssignable, Category = "Target System")
	FTraceComponentSetRotation OnTargetSetRotation;

	// Returns the reference to currently targeted Actor if any
	UFUNCTION(BlueprintCallable, Category = "Target System")
	AActor* GetLockedOnTargetActor() const;

	// Returns true / false whether the system is targeting an actor
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

	bool bIsBreakingLineOfSight = false;
	bool bIsSwitchingTarget = false;
	bool bTargetLocked = false;
	float ClosestTargetDistance = 0.0f;

	bool bDesireToSwitch = false;
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

	void EnableMidPointControlRotation(APlayerCharacter* PlayerOwner, const AActor* TargetActor);
	void DisableMidPointControlRotation();
	
	void CreateAndAttachTargetLockedOnWidgetComponent(AActor* TargetActor);
	
	void TargetLockOn(AActor* TargetToLockOn);
	void ResetIsSwitchingTarget();
	bool ShouldSwitchTargetActor(float AxisValue);

	static bool TargetIsTargetable(const AActor* Actor);
	
	 void SetupLocalPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite)
	bool IsTargeting = false;

	FVector MidPoint;
	float Radius;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> PlayerCharacter;
};
