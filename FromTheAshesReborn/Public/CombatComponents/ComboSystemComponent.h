#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboSystemComponent.generated.h"

class APlayableCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROMTHEASHESREBORN_API UComboSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

protected:

public:	

	UComboSystemComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
