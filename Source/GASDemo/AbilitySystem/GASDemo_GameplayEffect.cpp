// Fill out your copyright notice in the Description page of Project Settings.


#include "GASDemo_GameplayEffect.h"

UGASDemo_GameplayEffect::UGASDemo_GameplayEffect()
{
}

float UGASDemo_GameplayEffect::GetModifierMagnitudeByAttribute(const UGameplayEffect* Effect,
	const FGameplayAttribute& Attribute, float DefaultValue)
{
	if (!Effect)
	{
		return DefaultValue;
	}
    
	// Look through modifiers to find the one for this attribute
	for (const FGameplayModifierInfo& ModInfo : Effect->Modifiers)
	{
		if (ModInfo.Attribute == Attribute)
		{
			if (ModInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(1.0f, DefaultValue))
			{
				return DefaultValue;
			}
		}
	}
    
	return DefaultValue;
}
