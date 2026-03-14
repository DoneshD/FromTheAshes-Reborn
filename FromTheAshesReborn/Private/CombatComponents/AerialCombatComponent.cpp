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


	FDelegateHandle Handle = FTAAbilitySystemComponent->RegisterGameplayTagEvent(EnableTag, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UAerialCombatComponent::EnableComponent);
	
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
	IsComponentActive = false;
	CMC->GravityScale = 4.0f;
	TotalAirTime = 0.0f;
	LastGravityCurveValue = 0.0f;
	TestAttackCounter = 0.0f;

	FTAAbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("AerialCombatTag.AttackCounter")));
	EnableCollision();
	
}

void UAerialCombatComponent::InitializeStateAndVariables()
{
	UE_LOG(LogTemp, Warning, TEXT("Initializing"));
	IsComponentActive = true;
	CMC->AirControl = 0.10f;
	CMC->AirControlBoostMultiplier = 0.10f;
	
	DisableCollision();
	
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
	UE_LOG(LogTemp, Warning, TEXT("Curve val: %f"), LastGravityCurveValue);
	
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
