#include "Weapons/WeaponManagerComponent.h"
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
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		LightWeapon->AttachToComponent(OwnerPawn->GetMesh(), AttachmentRules, TEXT("hand_r_player_weapon_socket"));
	}
	
}

void UWeaponManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
									 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

