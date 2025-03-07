// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GASDemo_AbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASDEMO_API UGASDemo_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGASDemo_AbilitySystemComponent();
	
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	FGameplayAbilitySpecHandle GrantAbilityOfClass(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level = 1, bool bRemoveAfterActivation = false);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	AActor* GetCurrentActorInfo() const;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	bool bCharacterAbilitiesInitialized = false;
};
