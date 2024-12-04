// Fill out your copyright notice in the Description page of Project Settings.

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

	UPROPERTY(BlueprintReadWrite, Category = "Tracing")
	TObjectPtr<UDidItHitActorComponent> DidItHitActorComponent;

public:
	
	AWeaponActorBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
