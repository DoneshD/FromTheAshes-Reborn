#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FTAProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class FROMTHEASHESREBORN_API AFTAProjectile : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Core Components")
	TObjectPtr<USphereComponent> CollisionComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Core Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Core Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	

protected:
	
	AFTAProjectile(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
