#include "Enemy/EnemyBaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "NiagaraComponent.h"
#include "CombatComponents/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/AIControllerEnemyBase.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerCharacter.h"
#include "Weapon/EquipmentManagerComponent.h"
#include "Weapon/WeaponActorBase.h"

AEnemyBaseCharacter::AEnemyBaseCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	PlayerDetectionSphere =
	CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectionSphere"));

	PlayerDetectionSphere->InitSphereRadius(300.0f);
	PlayerDetectionSphere->SetupAttachment(GetCapsuleComponent());

	PlayerDetectionSphere->SetHiddenInGame(false);
	PlayerDetectionSphere->SetVisibility(true);
	
	// PlayerDetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// PlayerDetectionSphere->SetGenerateOverlapEvents(true);
	// PlayerDetectionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget->SetupAttachment(RootComponent);
	
	
}

void AEnemyBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	TargetObjectTraceChannel = ECC_GameTraceChannel2;

	UGroupCombatSubsystem* GCS = GetWorld()->GetSubsystem<UGroupCombatSubsystem>();

	if(!GCS)
	{
		UE_LOG(LogTemp, Error, TEXT("GCS is Null"))
		return;
	}

	// GCS->RegisterEnemyToGroupCombat(this);

	if(HealthWidget && HealthWidget->IsValidLowLevel())
	{
		HealthWidget->SetVisibility(false);	
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthWidget is Null or Invalid"));
	}

	

	if(!EquipmentManagerComponent && !EquipmentManagerComponent->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentManagerComponent is Null or Invalid"));
		return;
	}
	
	// EquipmentManagerComponent->SetEquippedWeapon(WeaponClass);

	if(!HealthComponent || !HealthWidget->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent is Null or Invalid"));
		return;
	}
	
	HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyBaseCharacter::HealthChanged);

	
	// FVector Location = PlayerDetectionSphere->GetComponentLocation();
	//
	// DrawDebugSphere(
	// 	GetWorld(),
	// 	Location,
	// 	PlayerDetectionSphere->GetScaledSphereRadius(),
	// 	32,
	// 	FColor::Red,
	// 	true,   // Persistent
	// 	-1.0f,  // Duration
	// 	0,
	// 	2.0f   // Thickness
	// );

	PlayerDetectionSphere->OnComponentBeginOverlap.AddDynamic(
		this,
		&AEnemyBaseCharacter::OnOverlapBegin
	);


	// if (FloatCurve)
	// {
	// 	FOnTimelineFloat ProgressFunction;
	// 	ProgressFunction.BindUFunction(this, FName("TimelineProgress"));
	//
	// 	FOnTimelineEvent FinishedFunction;
	// 	FinishedFunction.BindUFunction(this, FName("TimelineFinished"));
	//
	// 	DissolveTimeline.AddInterpFloat(FloatCurve, ProgressFunction);
	// 	DissolveTimeline.SetTimelineFinishedFunc(FinishedFunction);
	//
	// 	DissolveTimeline.SetLooping(false);
	// }

	// if(GetController())
	// {
	// 	AAIControllerEnemyGrunt* GruntController = Cast<AAIControllerEnemyGrunt>(GetController());
	//
	// 	if(GruntController && GruntController->IsValidLowLevel())
	// 	{
	// 		GruntController->Possess(this);
	// 		GruntController->StateTreeComponent->StartLogic();
	// 	}
	// }
	
	
}

void AEnemyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ShouldRotate)
	{
		if(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
			SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f));
			
		}
	}
	
	// if(ShouldDissolveTimelineTick)
	// {
	// 	DissolveTimeline.TickTimeline(DeltaTime);
	// }
	
}

void AEnemyBaseCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if(PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player detected"))
		OnPlayerInAggressionRadius.Broadcast(this, EEnemyEngagementRole::Aggressor);
	}
}

void AEnemyBaseCharacter::TimelineProgress(float Value)
{
	
}

void AEnemyBaseCharacter::TimelineFinished()
{
	// ShouldDissolveTimelineTick = false;
	GetMesh()->DestroyComponent();
	GetCapsuleComponent()->DestroyComponent();
	

}

void AEnemyBaseCharacter::HealthChanged(UHealthComponent* InHealthComponent, float OldValue, float NewValue, AActor* InInstigator)
{
	OnHealthChanged(InHealthComponent, OldValue, NewValue, InInstigator);
}

void AEnemyBaseCharacter::CheckDeath(float NewValue)
{
	bool TempSpecialBool = false;
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(PlayerChar)
	{
		if(PlayerChar->GetFTAAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("TestTag.Tag2")))
		{
			TempSpecialBool = true;
		}
	}
	if(!IsDead && NewValue <= 0.0f && !TempSpecialBool)
	{
		IsDead = true;
		Death();
	}
}

void AEnemyBaseCharacter::Death()
{
	AAIControllerEnemyBase* EnemyController = Cast<AAIControllerEnemyBase>(GetController());

	if(!EnemyController && !EnemyController->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid enemy controller"));
		return;
	}

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(Player)
	{
		Player->TempEnemyCountDeath += 1;
	}

	EnemyController->BrainComponent->StopLogic("");
	
	if(DeathMontage && DeathMontage->IsValidLowLevel())
	{
		ShouldRotate = false;
		
		PlayAnimMontage(DeathMontage);
		if (DeathSound)
		{
			float RandomPitch = UKismetMathLibrary::RandomFloatInRange(1.0f, 2.0f);
			UGameplayStatics::PlaySound2D(this, DeathSound, 1, RandomPitch);
		}
		
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

		for(auto Element : EquipmentManagerComponent->CurrentEquippedWeaponActors)
		{
			if(Element)
			{
				Element->Destroy();
			}
		}
		
		Destroy();

		OnDeath.Broadcast();
	
	}
}