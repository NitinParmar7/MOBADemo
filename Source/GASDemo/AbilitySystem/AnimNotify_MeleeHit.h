// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_MeleeHit.generated.h"

/**
 * 
 */
UCLASS()
class GASDEMO_API UAnimNotify_MeleeHit : public UAnimNotify
{
	
	GENERATED_BODY()
	
public:

	UAnimNotify_MeleeHit();
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,  const FAnimNotifyEventReference& EventReference) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Hit")
	FGameplayTag HitEventTag;
};
