#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActorBase.generated.h"

class UDidItHitActorComponent;

UCLASS()
class FROMTHEASHESREBORN_API AWeaponActorBase : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tracing")
	TObjectPtr<UDidItHitActorComponent> DidItHitActorComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

public:
	
	AWeaponActorBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartWeaponTracing();

	UFUNCTION(BlueprintCallable)
	void EndWeaponTracing();
	
};
