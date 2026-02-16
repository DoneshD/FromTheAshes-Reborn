#include "CombatComponents/AerialCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraSystemComponent.h"
#include "CombatComponents/DownedCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/AIControllerEnemyBase.h"
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

	DownedComp = FTACharacter->FindComponentByClass<UDownedCombatComponent>();

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
		if(DownedComp)
		{
			DownedComp->TotalDownedTime = 0.0f;
		}
		if(ActivateFromLauncher)
		{
			if(CMC->Velocity.Z <= 0.0f)
			{
				ActivateFromLauncher = false;
				// PrintGravity();
				TotalAirTime += DeltaTime;
				CMC->GravityScale = CalculateTimeSpentGravityMultiplier();
			}
			else
			{
				// PrintGravity();
				TotalAirTime += DeltaTime;
				CMC->GravityScale = CalculateTimeSpentGravityMultiplier();
			}
		}
		else
		{
			// PrintGravity();
			TotalAirTime += DeltaTime;
			CMC->GravityScale = CalculateTimeSpentGravityMultiplier();
		}
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

	// CameraParams.ArmLengthParams.ShouldAdjustArmLength = true;
	// CameraParams.ArmLengthParams.ShouldOverrideArmLength = true;
	// CameraParams.ArmLengthParams.ShouldResetOffset = true;
	// CameraParams.ArmLengthParams.DeltaArmLength = 400.0f;

	FTAAbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.AttackCounter")));
	EnableCollision();

	UCameraSystemComponent* CSC = GetOwner()->FindComponentByClass<UCameraSystemComponent>();
	if(!CSC)
	{
		return;
	}
	
	// CSC->HandleCameraSystemAdjustment(CameraParams);
	
}

void UAerialCombatComponent::InitializeStateAndVariables()
{
	IsComponentActive = true;

	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CMC->AirControl = 0.10f;
	CMC->AirControlBoostMultiplier = 0.10f;
	
	DisableCollision();
	ResetAttackTimer();
	
}

void UAerialCombatComponent::EnableComponent(const FGameplayTag InEnableTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		InitializeStateAndVariables();

		if (AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(FTACharacter->GetController()))
		{
			FGameplayTag HitTag = FGameplayTag::RequestGameplayTag("StateTreeTag.State.AirStunned");
		
			const UStateTreeComponent* STComp = EnemyController->StateTreeComponent;

			if (STComp)
			{
				FStateTreeEvent HitEvent;
				HitEvent.Tag = HitTag;
				EnemyController->StateTreeComponent->SendStateTreeEvent(HitEvent);
			}
		}
		
	}
	else
	{
		ClearStateAndVariables();

		if(!FTAAbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StateTreeTag.Status.State.AirStunned.Finished")))
		{
			FTAAbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("StateTreeTag.Status.State.AirStunned.Finished"));
			FTAAbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("StateTreeTag.Status.State.AirStunned.Finished"));
		}
		
	}
}

void UAerialCombatComponent::SetGravity(float NewGravity)
{
	CMC->GravityScale = NewGravity;
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
	float CurrentGravityScale = (TotalAirTime) * TimeGravityMultiplier;
	CurrentGravityScale = FMath::Clamp(CurrentGravityScale, MinimumGravityScale, MaximumGravityScale);
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

void UAerialCombatComponent::DisableCollision()
{
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);

}

void UAerialCombatComponent::EnableCollision()
{
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
	FTACharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);

}
