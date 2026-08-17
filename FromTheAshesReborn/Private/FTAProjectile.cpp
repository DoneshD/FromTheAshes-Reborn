#include "FTAProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DataAsset/HitReactionDataAsset.h"
#include "DataAsset/MoveToLocationDataAsset.h"
#include "DataAsset/RangedAbilityDataAsset.h"
#include "FTAAbilitySystem/GameplayAbilities/Hit/GA_ReceiveHit.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HelperFunctionLibraries/TagValidationFunctionLibrary.h"
#include "Player/PlayerCharacter.h"


AFTAProjectile::AFTAProjectile(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(15.0f);
	RootComponent = CollisionComponent;
	

	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	StaticMeshComponent =  CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	
}

void AFTAProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AFTAProjectile::OnOverlapBegin);
	
}

void AFTAProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFTAProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if(!PlayerCharacter)
	{
		return;
	}
	
	
	TargetHitDataHandle = AddHitResultToTargetData(SweepResult);
	if(TargetHitDataHandle.Num() > 0 && TargetHitDataHandle.Get(0))
	{
		ExecuteHitLogic(TargetHitDataHandle);
	}
}

FGameplayAbilityTargetDataHandle AFTAProjectile::AddHitResultToTargetData(const FHitResult& LastItem)
{
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(LastItem);

	if(!TargetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Target Data"))
		return nullptr;
	}
	
	TargetData->HitResult = LastItem;
	
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(TargetData);
	return TargetDataHandle;
}

void AFTAProjectile::ExecuteHitLogic(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();

	if(!TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetActor null"));
		return;
	}
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);

	if(!TargetASC)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTAProjectile::ExecuteHitLogic - TargetASC null"));
		return;
	}

	const FGameplayAbilityActorInfo* TargetActorInfo = TargetASC->AbilityActorInfo.Get();
	
	if (IsValid(RangedAbilityDataAsset) && !RangedAbilityDataAsset->PossibleHitReactions.IsEmpty())
	{
		for(FHitDataInfo& HitData : RangedAbilityDataAsset->PossibleHitReactions)
		{
			if(HitData.HitAbilityClass && HitData.HitAbilityClass->IsValidLowLevel())
			{
				const UGA_ReceiveHit* const CDO = HitData.HitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
				if (CDO)
				{
					GrantHitAbility(TargetDataHandle, HitData.HitAbilityClass);
					const FGameplayAbilitySpec* TargetSpec = TargetASC->FindAbilitySpecFromClass(HitData.HitAbilityClass);
					if(TargetSpec)
					{
						
						if(CDO->CanActivateAbility(TargetSpec->Handle, TargetActorInfo, nullptr, nullptr, nullptr))
						{
							ApplyHitEffects(TargetDataHandle, HitData.HitAbilityClass);
							SendHitGameplayEvents(TargetDataHandle, HitData);
							AddHitCues(TargetDataHandle, HitData.HitAbilityClass);
							break;
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("AFTAProjectile::ExecuteHitLogic: Invalid spec from class "))
					}
				}
			}
		}
	}
}

void AFTAProjectile::GrantHitAbility(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	if (InHitAbilityClass->IsValidLowLevel())
	{
		const UGA_ReceiveHit* const CDO = InHitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->ReceiveHitTag))
			{
				const TSubclassOf<UGameplayEffect>* GrantAbilityEffect = CDO->ReceiveHitEffectMap.Find(CDO->ReceiveHitTag);

				if(GrantAbilityEffect)
				{
					AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();
					if(TargetActor)
					{
						UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor);
						
						FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
						EffectContext.AddInstigator(SourceActor, this);
						
						UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
						
						if(TargetASC)
						{
							FGameplayEffectSpecHandle GrantAbilityEffectHandle = SourceASC->MakeOutgoingSpec(
								*GrantAbilityEffect,
								1.0f,               
								EffectContext       
							);

							if (GrantAbilityEffectHandle.IsValid())
							{
								TargetASC->ApplyGameplayEffectSpecToSelf(*GrantAbilityEffectHandle.Data.Get());
							}
						}
					}

				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("AFTAProjectile::GrantHitAbility - GrantAbilityEffect is null"))
					return;
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("AFTAProjectile::GrantHitAbility - Invalid tag"))
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AFTAProjectile::GrantHitAbility - Invalid CDO"))
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AFTAProjectile::GrantHitAbility - InHitAbilityClass is invalid"));
		return;
	}
}

void AFTAProjectile::ApplyHitEffects(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
	
}

void AFTAProjectile::AddHitCues(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	TSubclassOf<UGA_ReceiveHit> InHitAbilityClass)
{
}

void AFTAProjectile::SendHitGameplayEvents(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	FHitDataInfo& HitData)
{
	AActor* TargetActor = TargetDataHandle.Get(0)->GetHitResult()->GetActor();

	AFTACharacter* TargetCharacter = Cast<AFTACharacter>(TargetActor);
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	OnHitEventData.Instigator = SourceActor;
	OnHitEventData.Target = TargetActor;
	OnHitEventData.ContextHandle.AddHitResult(*TargetDataHandle.Get(0)->GetHitResult());
	
	UHitEventObject* HitInfoObj = NewObject<UHitEventObject>(this);
	HitInfoObj->HitData.Instigator =SourceActor;
	
	HitInfoObj->HitData.Direction = HitData.Direction;
	
	if(!HitData.ChainReactions.IsEmpty())
	{
		HitInfoObj->HitData.ChainReactions = HitData.ChainReactions;
	}
	
	
	
	if (HitData.HitAbilityClass)
	{
		const UGA_ReceiveHit* const CDO = HitData.HitAbilityClass->GetDefaultObject<UGA_ReceiveHit>();
		if (CDO)
		{
			if(UTagValidationFunctionLibrary::IsRegisteredGameplayTag(CDO->ReceiveHitTag))
			{
				if(!HitInfoObj->HitData.MoveToLocationData)
				{
					if(CDO->DefaultMoveToLocationDataAsset)
					{
						HitInfoObj->HitData.MoveToLocationData = DuplicateObject<UMoveToLocationDataAsset>(CDO->DefaultMoveToLocationDataAsset,this);
					}
				}
				OnHitEventData.EventTag = CDO->ReceiveHitTag;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UGA_MeleeWeaponAttack::SendMeleeHitGameplayEvents - HitReactionTag is invalid"));
			}
		}
	}

	if(HitData.MoveToLocationData)
	{
		HitInfoObj->HitData.MoveToLocationData = HitData.MoveToLocationData;
	}
	
	if(CurrentMoveToLocationAsset)
	{
		if(RangedAbilityDataAsset->SupplyEndLocation)
		{
			HitInfoObj->HitData.MoveToLocationData->LocationData.EndLocationVector = CurrentMoveToLocationAsset->TempLocationData.EndLocationVector;
			HitInfoObj->HitData.MoveToLocationData->LocationData.Duration = CurrentMoveToLocationAsset->LocationData.Duration;
		}
	}
	
	if(!OnHitEventData.OptionalObject)
	{
		OnHitEventData.OptionalObject = HitInfoObj;
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, OnHitEventData.EventTag, OnHitEventData);
	OnHitEventData.OptionalObject = nullptr;
	if(CurrentMoveToLocationAsset)
	{
		CurrentMoveToLocationAsset->LocationData.EndLocationVector = FVector::ZeroVector;
		// CurrentMoveToLocationAsset->LocationData.RelativeOffsetVector = FVector::ZeroVector;
		CurrentMoveToLocationAsset->TempLocationData.EndLocationVector = FVector::ZeroVector;
	}
}

