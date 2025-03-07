// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GASDemo_GameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UGASDemo_GameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

	public:

	UGASDemo_GameplayEffect();

	UFUNCTION(BlueprintCallable, Category = "Gameplay Effect")
	static float GetModifierMagnitudeByAttribute(const UGameplayEffect* Effect, 
												const FGameplayAttribute& Attribute, 
												float DefaultValue = 0.0f);
};
