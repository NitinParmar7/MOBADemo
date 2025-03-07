// Fill out your copyright notice in the Description page of Project Settings.


#include "GASDemo_AbilitySystemComponent.h"


// Sets default values for this component's properties
UGASDemo_AbilitySystemComponent::UGASDemo_AbilitySystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGASDemo_AbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UGASDemo_AbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

FGameplayAbilitySpecHandle UGASDemo_AbilitySystemComponent::GrantAbilityOfClass(
	TSubclassOf<UGameplayAbility> AbilityClass, int32 Level, bool bRemoveAfterActivation)
{
	if (!AbilityClass || GetOwnerRole() != ROLE_Authority)
	{
		return FGameplayAbilitySpecHandle();
	}
    
	FGameplayAbilitySpec AbilitySpec(AbilityClass, Level, INDEX_NONE, GetAvatarActor());
	AbilitySpec.RemoveAfterActivation = bRemoveAfterActivation;
    
	return GiveAbility(AbilitySpec);
}

AActor* UGASDemo_AbilitySystemComponent::GetCurrentActorInfo() const
{
	return GetAvatarActor();
}


// Called every frame
void UGASDemo_AbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

