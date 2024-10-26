#include "Weapons/WeaponManagerComponent.h"

#include "DidItHitActorComponent.h"
#include "GameFramework/Character.h"
#include "Weapons/FTAWeapon.h"


UWeaponManagerComponent::UWeaponManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* OwnerPawn = Cast<ACharacter>(GetOwner());

	if (LightWeapon = GetWorld()->SpawnActor<AFTAWeapon>(LightWeaponClass, OwnerPawn->GetMesh()->GetSocketTransform(TEXT("hand_r_player_weapon_socket"))))
	{
		if(LightWeapon)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
			LightWeapon->AttachToComponent(OwnerPawn->GetMesh(), AttachmentRules, TEXT("hand_r_player_weapon_socket"));
			LightWeapon->DidItHitActorComponent->SetupVariables(LightWeapon->GetWeaponMesh(), GetOwner());
			LightWeapon->DidItHitActorComponent->OnItemAdded.AddDynamic(this, &UWeaponManagerComponent::UWeaponManagerComponent::OnHitAdded);
		}
	}
	
}

void UWeaponManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
									 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWeaponManagerComponent::StartWeaponTrace()
{
	if(!LightWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Light weapon is NULL"));
		return;
	}
	
	LightWeapon->DidItHitActorComponent->ToggleTraceCheck(true);
}

void UWeaponManagerComponent::EndWeaponTrace()
{
	if(!LightWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Light weapon is NULL"));
		return;
	}
	
	for(const FHitResult& Hit : LightWeapon->DidItHitActorComponent->HitArray)
	{
		FName Name = Hit.GetActor()->GetFName();
		UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *Name.ToString());
	}
	
	LightWeapon->DidItHitActorComponent->ToggleTraceCheck(false);
	
}

void UWeaponManagerComponent::OnHitAdded(FHitResult LastItem)
{
	UE_LOG(LogTemp, Warning, TEXT("HIT!"))
	
}

AFTAWeapon* UWeaponManagerComponent::GetLightWeapon()
{
	if(LightWeapon)
	{
		return LightWeapon;
	}
	
	UE_LOG(LogTemp, Error, TEXT("Light Weapon Is NULL"));
	return nullptr;
	
}

AFTAWeapon* UWeaponManagerComponent::GetHeavyWeapon()
{
	if(HeavyWeapon)
	{
		return HeavyWeapon;
	}
	
	UE_LOG(LogTemp, Error, TEXT("Heavy Weapon Is NULL"));
	return nullptr;
}

