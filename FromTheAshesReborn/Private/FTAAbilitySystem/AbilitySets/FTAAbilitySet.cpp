#include "FTAAbilitySystem/AbilitySets/FTAAbilitySet.h"
#include "FTAAbilitySystem/GameplayAbilities/FTAGameplayAbility.h"
#include "FTAAbilitySystem/AbilitySystemComponent/FTAAbilitySystemComponent.h"
#include "FTAAbilitySystem/AttributeSets/FTAAttributeSet.h"

void FFTAAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FFTAAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FFTAAbilitySet_GrantedHandles::AddAttributeSet(UFTAAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FFTAAbilitySet_GrantedHandles::TakeFromAbilitySystem(UFTAAbilitySystemComponent* FTAASC)
{
	if(!FTAASC)
	{
		UE_LOG(LogTemp, Error, TEXT("FFTAAbilitySet_GrantedHandles::TakeFromAbilitySystem - FTAASC is invalid"))
		return;
	}

	if (!FTAASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			FTAASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			FTAASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		FTAASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UFTAAbilitySet::UFTAAbilitySet(const FObjectInitializer& ObjectInitializer)
{
	
}

void UFTAAbilitySet::GiveToAbilitySystem(UFTAAbilitySystemComponent* FTAASC, FFTAAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	if(!FTAASC)
	{
		UE_LOG(LogTemp, Error, TEXT("AFTACharacter::InitAbilitySystemComponent - FTAASC is invalid"))
		return;
	}
	
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FFTAAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
	
		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}
	
		UFTAGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UFTAGameplayAbility>();
	
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);
	
		const FGameplayAbilitySpecHandle AbilitySpecHandle = FTAASC->GiveAbility(AbilitySpec);
	
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
	
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FFTAAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];
	
		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}
	
		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = FTAASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, FTAASC->MakeEffectContext());
	
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
	
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FFTAAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];
	
		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}
	
		UFTAAttributeSet* NewSet = NewObject<UFTAAttributeSet>(FTAASC->GetOwner(), SetToGrant.AttributeSet);
		FTAASC->AddAttributeSetSubobject(NewSet);
	
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}
