// GA_Dash.h
#pragma once

#include "CoreMinimal.h"
#include "GASDemo_GameplayAbility.h"
#include "GA_Dash.generated.h"

UCLASS()
class GASDEMO_API UGA_Dash : public UGASDemo_GameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Dash();
    
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

	// Callback for dash completion
	UFUNCTION()
	void OnDashCompleted();

	// Dash parameters
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDistance = 500.0f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration = 0.5f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashSpeedMultiplier = 2.0f;
    
	// Cooldown duration
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownDuration = 3.0f;
    
	// Cost Gameplay Effect
	UPROPERTY(EditDefaultsOnly, Category = "Cost")
	TSubclassOf<UGameplayEffect> CostEffectClass;
    
	// Cooldown Gameplay Effect
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	TSubclassOf<UGameplayEffect> CooldownEffectClass;
    
	// Original walk speed
	float OriginalWalkSpeed = 0.0f;
};