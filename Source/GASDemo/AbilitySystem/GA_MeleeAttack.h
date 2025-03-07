// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASDemo_GameplayAbility.h"
#include "GA_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGA_MeleeAttack : public UGASDemo_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_MeleeAttack();
    
	// Implement ability
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
							   const FGameplayAbilityActorInfo* ActorInfo, 
							   const FGameplayAbilityActivationInfo ActivationInfo,
							   const FGameplayEventData* TriggerEventData) override;
    
	// End ability implementation
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, 
						  const FGameplayAbilityActorInfo* ActorInfo, 
						  const FGameplayAbilityActivationInfo ActivationInfo, 
						  bool bReplicateEndAbility, bool bWasCancelled) override;
    
	// Perform melee attack
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void PerformMeleeAttack();
    
	// Gameplay Effect to apply
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
    
	// Attack range
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackRange = 150.0f;
    
	// Attack radius for sweep
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackRadius = 50.0f;
    
	// Base damage
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float BaseDamage = 20.0f;
};
