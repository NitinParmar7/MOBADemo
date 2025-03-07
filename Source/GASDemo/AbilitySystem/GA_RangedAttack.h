// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASDemo_GameplayAbility.h"
#include "GA_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGA_RangedAttack : public UGASDemo_GameplayAbility
{
	GENERATED_BODY()

public:
	UGA_RangedAttack();
    
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
    
	// Spawn projectile
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SpawnProjectile();
    
	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AActor> ProjectileClass;
    
	// Projectile spawn offset
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FVector SpawnOffset = FVector(50.0f, 0.0f, 0.0f);
    
	// Mana cost
	UPROPERTY(EditDefaultsOnly, Category = "Cost")
	float ManaCost = 20.0f;
    
	// Cooldown duration
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownDuration = 2.0f;
    
	// Cost Gameplay Effect
	UPROPERTY(EditDefaultsOnly, Category = "Cost")
	TSubclassOf<UGameplayEffect> CostEffectClass;
    
	// Cooldown Gameplay Effect
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffectClass;
};
