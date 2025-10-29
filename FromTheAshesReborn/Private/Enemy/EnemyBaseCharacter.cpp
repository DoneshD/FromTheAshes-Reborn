#include "Enemy/EnemyBaseCharacter.h"

#include "NiagaraComponent.h"
#include "CombatComponents/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/AIControllerEnemyBase.h"
#include "Enemy/GroupCombatSubsystem.h"
#include "FTACustomBase/FTACharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/EquipmentManagerComponent.h"

AEnemyBaseCharacter::AEnemyBaseCharacter(const class FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UFTACharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget->SetupAttachment(RootComponent);

	DissolveNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DissolveNiagaraComponent"));
	DissolveNiagaraComponent->SetupAttachment(GetMesh());

	DissolveMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DissolveMeshComponent"));
	DissolveMeshComponent->SetupAttachment(GetMesh());
	
}

void AEnemyBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGroupCombatSubsystem* GCS = GetWorld()->GetSubsystem<UGroupCombatSubsystem>();

	if(!GCS)
	{
		UE_LOG(LogTemp, Error, TEXT("GCS is Null"))
		return;
	}

	GCS->RegisterEnemyToGroupCombat(this);

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
	
	EquipmentManagerComponent->SetEquippedWeapon(WeaponClass);

	if(!HealthComponent || !HealthWidget->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent is Null or Invalid"));
		return;
	}
	
	HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyBaseCharacter::HealthChanged);

	if (FloatCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("TimelineProgress"));

		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindUFunction(this, FName("TimelineFinished"));

		MyTimeline.AddInterpFloat(FloatCurve, ProgressFunction);
		MyTimeline.SetTimelineFinishedFunc(FinishedFunction);

		MyTimeline.SetLooping(false);
		MyTimeline.PlayFromStart();
	}
	
}

void AEnemyBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ShouldRotate)
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation());
		SetActorRotation(FRotator(0.0f, LookAtRotation.Yaw, 0.0f));
	}

	MyTimeline.TickTimeline(DeltaTime);
	
}

void AEnemyBaseCharacter::TimelineProgress(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("Timeline progress: %f"), Value);
}

void AEnemyBaseCharacter::TimelineFinished()
{
	
}

void AEnemyBaseCharacter::HealthChanged(UHealthComponent* InHealthComponent, float OldValue, float NewValue, AActor* InInstigator)
{
	OnHealthChanged(InHealthComponent, OldValue, NewValue, InInstigator);
}

void AEnemyBaseCharacter::CheckDeath(float NewValue)
{
	if(!IsDead && NewValue <= 0.0f)
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

	EnemyController->BrainComponent->StopLogic("");
	
	if(DeathMontage && DeathMontage->IsValidLowLevel())
	{
		ShouldRotate = false;
		PlayAnimMontage(DeathMontage);
		
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
		
	
	}
}


void AEnemyBaseCharacter::SetDissolveParams()
{
	CodeDynamicDissolveMaterial1 = GetMesh()->CreateDynamicMaterialInstance(0, CodeDissolveMaterial1);
	CodeDynamicDissolveMaterial2 = GetMesh()->CreateDynamicMaterialInstance(1, CodeDissolveMaterial2);
	CodeDynamicDissolveMaterial3 = GetMesh()->CreateDynamicMaterialInstance(2, CodeDissolveMaterial2);
	
	DissolveNiagaraComponent->SetAsset(CodeSetDissolveNiagara, true);

	DissolveNiagaraComponent->SetVariableFloat(TEXT("DissolveAmount"), CodeDissolveAmount);

	DissolveNiagaraComponent->SetVariableVec3(TEXT("Position"), DissolveMeshComponent->GetComponentLocation());
	DissolveNiagaraComponent->SetVariableTexture(TEXT("DissolveTexture"), CodeDissolveTexture);
	DissolveNiagaraComponent->SetVariableVec2(TEXT("DissolveTextureUV"), CodeDissolveTextureUV);
	
	CodeDynamicDissolveMaterial1->SetScalarParameterValue(TEXT("DissolveAmount"), CodeDissolveAmount);
	CodeDynamicDissolveMaterial1->SetVectorParameterValue(TEXT("Position"), DissolveMeshComponent->GetComponentLocation());
	
	CodeDynamicDissolveMaterial1->SetTextureParameterValue(TEXT("DissolveTexture"), CodeDissolveTexture);
	CodeDynamicDissolveMaterial1->SetVectorParameterValue(TEXT("DissolveTextureUV"), FVector(CodeDissolveTextureUV.X, CodeDissolveTextureUV.Y, 0.0f));
	
	CodeDynamicDissolveMaterial2->SetScalarParameterValue(TEXT("DissolveAmount"), CodeDissolveAmount);
	CodeDynamicDissolveMaterial2->SetVectorParameterValue(TEXT("Position"), DissolveMeshComponent->GetComponentLocation());

	CodeDynamicDissolveMaterial2->SetTextureParameterValue(TEXT("DissolveTexture"), CodeDissolveTexture);
	CodeDynamicDissolveMaterial2->SetVectorParameterValue(TEXT("DissolveTextureUV"), FVector(CodeDissolveTextureUV.X, CodeDissolveTextureUV.Y, 0.0f));

	CodeDynamicDissolveMaterial3->SetScalarParameterValue(TEXT("DissolveAmount"), CodeDissolveAmount);
	CodeDynamicDissolveMaterial3->SetVectorParameterValue(TEXT("Position"), DissolveMeshComponent->GetComponentLocation());

	CodeDynamicDissolveMaterial3->SetTextureParameterValue(TEXT("DissolveTexture"), CodeDissolveTexture);
	CodeDynamicDissolveMaterial3->SetVectorParameterValue(TEXT("DissolveTextureUV"), FVector(CodeDissolveTextureUV.X, CodeDissolveTextureUV.Y, 0.0f));

	Dissolve();
	
	
}

void AEnemyBaseCharacter::Dissolve()
{
	MyTimeline.Play();

	
}



