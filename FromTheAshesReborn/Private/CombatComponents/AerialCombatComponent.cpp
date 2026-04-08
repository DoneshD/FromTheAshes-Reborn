#include "CombatComponents/AerialCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CombatComponents/CentralStateComponent.h"
#include "Components/CapsuleComponent.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


UAerialCombatComponent::UAerialCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAerialCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	FTACharacter = Cast<AFTACharacter>(GetOwner());

	if(!FTACharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UAerialCombatComponent::BeginPlay() - FTACharacter is null"));
		return;
	}

	FTAAbilitySystemComponent = FTACharacter->GetFTAAbilitySystemComponent();

	if(!FTAAbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UAerialCombatComponent::BeginPlay() - FTAAbilitySystemComponent is null"));
		return;
	}

	CMC = Cast<UCharacterMovementComponent>(FTACharacter->GetMovementComponent());

	if(!CMC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAerialCombatComponent::BeginPlay() - CMC is Null"));
		return;
	}
	
	
}

void UAerialCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(IsComponentActive)
	{
		TotalAirTime += DeltaTime;
		if(GravityCurve)
		{
			LastGravityCurveValue = GravityCurve->GetFloatValue(TotalAirTime); 
			CMC->GravityScale = GravityCurve->GetFloatValue(TotalAirTime);
		}
		else
		{
			CMC->GravityScale = 0.0f;
		}
		// PrintGravity();
	}
}

void UAerialCombatComponent::ClearStateAndVariables()
{
	UCentralStateComponent* CSC = GetOwner()->FindComponentByClass<UCentralStateComponent>();
	if(CSC)
	{
		CSC->SetCurrentOrientation(CSC->GroundedOrientationTag, MOVE_Walking);
	}
	
	IsComponentActive = false;
	CMC->GravityScale = 4.0f;
	TotalAirTime = 0.0f;
	LastGravityCurveValue = 0.0f;
	TestAttackCounter = 0.0f;

	EnableCollision();
	
}

void UAerialCombatComponent::InitializeStateAndVariables(EMovementMode MovementMode)
{
	UCentralStateComponent* CSC = GetOwner()->FindComponentByClass<UCentralStateComponent>();
	if(CSC)
	{
		CSC->SetCurrentOrientation(CSC->AirborneOrientationTag, MovementMode);
	}
	
	IsComponentActive = true;
	CMC->AirControl = 0.10f;
	CMC->AirControlBoostMultiplier = 0.10f;
	
	DisableCollision();
	
}

void UAerialCombatComponent::EnableComponent(TEnumAsByte<EMovementMode> MovementMode)
{
	InitializeStateAndVariables(MovementMode);
}

void UAerialCombatComponent::PrintGravity()
{
	if (AActor* Owner = GetOwner())
	{
		FString OwnerName = Owner->GetName();
		UE_LOG(LogTemp, Warning, TEXT("[%s] Gravity: %f"), *OwnerName, CMC->GravityScale);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[No Owner] Gravity: %f"), CMC->GravityScale);
	}
}

void UAerialCombatComponent::AbilityInitTest()
{
	if(TestAttackCounter < 4)
	{
		TotalAirTime = 0.0f;
	}
	
}

void UAerialCombatComponent::DisableCollision()
{
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

}

void UAerialCombatComponent::EnableCollision()
{
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

}
