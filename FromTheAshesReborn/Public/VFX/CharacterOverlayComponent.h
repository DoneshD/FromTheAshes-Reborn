#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterOverlayComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UCharacterOverlayComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UCharacterOverlayComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
