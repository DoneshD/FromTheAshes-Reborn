#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"

UFTAAttributeSet::UFTAAttributeSet()
{

}
UFTAAbilitySystemComponent* UFTAAttributeSet::GetFTAAbilitySystemComponent()
{
	return Cast<UFTAAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

