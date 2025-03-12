// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_MeleeHit.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GASDemo/GASDemoCharacter.h"

UAnimNotify_MeleeHit::UAnimNotify_MeleeHit()
{
	HitEventTag = FGameplayTag::RequestGameplayTag(FName("Ability.AnimNotify.MeleeHit"));
}

void UAnimNotify_MeleeHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation);
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	if (AGASDemoCharacter* Character = Cast<AGASDemoCharacter>(MeshComp->GetOwner()))
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, HitEventTag, FGameplayEventData());
	}
}
