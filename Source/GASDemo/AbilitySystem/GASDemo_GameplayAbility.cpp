// ARPGGameplayAbility.cpp
#include "GASDemo_GameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GASDemo/GASDemoCharacter.h"

UGASDemo_GameplayAbility::UGASDemo_GameplayAbility()
{
	// Default constructor
	bActivateOnGranted = false;
    
	// Default tags
	EffectGameplayCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Ability.Generic"));
}

void UGASDemo_GameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
    
	// Auto-activate if needed
	if (bActivateOnGranted && ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		bool bActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

AGASDemoCharacter* UGASDemo_GameplayAbility::GetCharacterFromActorInfo() const
{
	return Cast<AGASDemoCharacter>(GetAvatarActorFromActorInfo());
}

float UGASDemo_GameplayAbility::PlayAbilityMontage(UAnimMontage* Montage, float PlayRate, FName StartSectionName)
{
	if (!Montage || !GetCharacterFromActorInfo())
	{
		return 0.0f;
	}
    
	float Duration = GetCharacterFromActorInfo()->PlayAnimMontage(Montage, PlayRate, StartSectionName);
	return Duration;
}

bool UGASDemo_GameplayAbility::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(TagToCheck);
}

bool UGASDemo_GameplayAbility::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GetAbilitySystemComponentFromActorInfo()->HasAnyMatchingGameplayTags(TagContainer);
}
