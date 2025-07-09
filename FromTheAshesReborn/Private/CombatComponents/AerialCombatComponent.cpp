#include "CombatComponents/AerialCombatComponent.h"

#include "CameraSystemComponent.h"
#include "CameraSystemParams.h"
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

	FDelegateHandle Handle = FTAAbilitySystemComponent->RegisterGameplayTagEvent(EnableTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UAerialCombatComponent::EnableComponent);

	FDelegateHandle HandleTwo = FTAAbilitySystemComponent->RegisterGameplayTagEvent(AerialAttackCounterTag, EGameplayTagEventType::AnyCountChange)
		.AddUObject(this, &UAerialCombatComponent::AddAttackCounterTag);
	
}

void UAerialCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(IsComponentActive)
	{
		TotalAirTime += DeltaTime;
		CMC->GravityScale = CalculateTimeSpentGravityMultiplier();
	}
}

void UAerialCombatComponent::ClearStateAndVariables()
{
	IsComponentActive = false;
	CMC->GravityScale = 4.0f;
	AttackCounter = 0;
	AttackLastResetTime = GetWorld()->GetTimeSeconds();
	TotalAirTime = 0.0f;
	
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	FCameraSystemParams CameraParams;
	CameraParams.ShouldAdjustArmLength = true;
	CameraParams.DeltaArmLength = -300.0f;

	UCameraSystemComponent* CSC = GetOwner()->FindComponentByClass<UCameraSystemComponent>();
	if(!CSC)
	{
		UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack_Launcher::ActivateAbility - CameraSystemComponent"));
		return;
	}
	
	CSC->HandleCameraSystemAdjustment(CameraParams);

	FTAAbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.AttackCounter")));

}

void UAerialCombatComponent::InitializeStateAndVariables()
{
	IsComponentActive = true;

	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	ResetAttackTimer();
	
}

void UAerialCombatComponent::EnableComponent(const FGameplayTag InEnableTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		InitializeStateAndVariables();
	}
	else
	{
		ClearStateAndVariables();
	}
}

void UAerialCombatComponent::SetGravity(float NewGravity)
{
	CMC->GravityScale = NewGravity;
}

void UAerialCombatComponent::PrintGravity()
{
	UE_LOG(LogTemp, Warning, TEXT("Gravity: %f"), CMC->GravityScale)
}

void UAerialCombatComponent::AddAttackCounterTag(const FGameplayTag InAttackCounterTag, int32 NewCount)
{
	if (NewCount > PreviousCount)
	{
		CalculateAttackAntiGravityMultiplier(NewCount);
	}

	PreviousCount = NewCount;
}

float UAerialCombatComponent::CalculateAttackAntiGravityMultiplier(int InNewCount)
{
	AttackCounter = InNewCount;
	
	if (AttackCounter <= 3)
	{
		LaunchStrength = 100.0f;
	}
	else if (AttackCounter > 3 && AttackCounter <= 6)
	{
		LaunchStrength = 200.0f;
	}
	else if (AttackCounter > 6 && AttackCounter <= 9)
	{
		LaunchStrength = 300.0f;
	}
	else
	{
		CMC->GravityScale = 4.0f;
		LaunchStrength = 0.0f;
	}

	ResetAttackTimer();

	FVector LaunchVelocity = FVector(0.0f, 0.0f, LaunchStrength);
	CMC->Velocity = FVector::ZeroVector;
	FTACharacter->LaunchCharacter(LaunchVelocity, true, true);

	return 0.0f;
}


float UAerialCombatComponent::CalculateTimeSpentGravityMultiplier() const
{
	float CurrentGravityScale = FMath::Square(TotalAirTime) * TimeGravityMultiplier;
	CurrentGravityScale = FMath::Clamp(CurrentGravityScale, 1.0, 4.0f);
	return CurrentGravityScale;
}

void UAerialCombatComponent::ResetAttackTimer()
{
	AttackLastResetTime = GetWorld()->GetTimeSeconds();
}

float UAerialCombatComponent::GetAttackElapsedTime() const
{
	return GetWorld()->GetTimeSeconds() - AttackLastResetTime;
}