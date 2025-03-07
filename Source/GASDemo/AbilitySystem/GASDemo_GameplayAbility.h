// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASDemo_GameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGASDemo_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGASDemo_GameplayAbility();

	// Input tag for ability activation
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTag AbilityInputTag;

	// Visual effect to spawn on ability activation
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ActivationEffect;

	// Sound to play on activation
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundBase* ActivationSound;
    
	// Gameplay cue tag to trigger effects
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGameplayTag EffectGameplayCueTag;
    
	// Montage to play for this ability
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* AbilityMontage;
    
	// Whether this ability should auto activate
	UPROPERTY(EditDefaultsOnly, Category = "Activation")
	bool bActivateOnGranted;
    
	// Override to add additional init
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    
	// Helper to get owning character
	UFUNCTION(BlueprintCallable, Category = "Ability")
	class AGASDemoCharacter* GetCharacterFromActorInfo() const;
    
	// Helper to play montage from ability
	UFUNCTION(BlueprintCallable, Category = "Ability|Animation")
	float PlayAbilityMontage(UAnimMontage* Montage, float PlayRate = 1.0f, FName StartSectionName = NAME_None);
    
	// Helper to find ability tags
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const;
    
	// Helper to find ability tags from container
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const;
};
