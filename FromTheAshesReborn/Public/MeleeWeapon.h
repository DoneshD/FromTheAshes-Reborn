#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeleeWeapon.generated.h"

class UStaticMeshComponent;

UCLASS()
class FROMTHEASHESREBORN_API AMeleeWeapon : public AActor
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> MeleeWeaponMesh;

public:	
	AMeleeWeapon();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
