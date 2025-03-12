// GA_Dash.cpp
#include "GA_Dash.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GASDemo/GASDemoCharacter.h"

UGA_Dash::UGA_Dash()
{
    // Default constructor
    
    // Set ability tags
    AbilityInputTag = FGameplayTag::RequestGameplayTag(FName("Ability.Movement.Dash"));
    
    // Setup gameplay tags
    FGameplayTagContainer GameplayTagContainer;
    GameplayTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Movement.Dash")));
    SetAssetTags(GameplayTagContainer);
    
    // Setup cancellation tags
    FGameplayTagContainer CancelTags;
    CancelTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack")));
    BlockAbilitiesWithTag = CancelTags;
    
    
    // Effect gameplay cue
    EffectGameplayCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Movement.Dash"));
    
    // Set instancing policy
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    
    // Set network policy
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    
    // Default parameters
    DashDistance = 500.0f;
    DashDuration = 0.5f;
    DashSpeedMultiplier = 2.0f;
    CooldownDuration = 3.0f;
}

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
                             const FGameplayAbilityActorInfo* ActorInfo, 
                             const FGameplayAbilityActivationInfo ActivationInfo, 
                             const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    // Get character
    AGASDemoCharacter* Character = GetCharacterFromActorInfo();
    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    // Store original walk speed
    if (Character->GetCharacterMovement())
    {
        OriginalWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
        
        // Increase speed for dash
        Character->GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed * DashSpeedMultiplier;
    }
    
    // Apply root motion force
    UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
        this,
        TEXT("Dash"),
        Character->GetActorForwardVector(),
        DashDistance / DashDuration,
        DashDuration,
        false,
        nullptr,
        ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
        FVector::ZeroVector,
        0.0f,
        false
    );
    
    // Bind to completed event
    if (RootMotionTask)
    {
        RootMotionTask->OnFinish.AddDynamic(this, &UGA_Dash::OnDashCompleted);
        RootMotionTask->ReadyForActivation();
    }
    else
    {
        // No task, end ability
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
    
    // Trigger gameplay cue
    if (EffectGameplayCueTag.IsValid())
    {
        FGameplayCueParameters CueParams;
        CueParams.Location = Character->GetActorLocation();
        CueParams.Normal = Character->GetActorForwardVector();
        CueParams.SourceObject = Character;
        
        GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(EffectGameplayCueTag, CueParams);
    }
    
    // Play montage if available
    if (AbilityMontage)
    {
        PlayAbilityMontage(AbilityMontage, 1.0f);
    }
}

void UGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, 
                        const FGameplayAbilityActorInfo* ActorInfo, 
                        const FGameplayAbilityActivationInfo ActivationInfo, 
                        bool bReplicateEndAbility, 
                        bool bWasCancelled)
{
    // Reset character speed
    AGASDemoCharacter* Character = GetCharacterFromActorInfo();
    if (Character && Character->GetCharacterMovement() && OriginalWalkSpeed > 0.0f)
    {
        Character->GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed;
    }
    
    // Apply cooldown if not cancelled
    if (!bWasCancelled && CooldownEffectClass)
    {
        FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
        EffectContext.AddSourceObject(Character);
        
        FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
            CooldownEffectClass,
            GetAbilityLevel(),
            EffectContext
        );
        
        if (SpecHandle.IsValid())
        {
            GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
    
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UGA_Dash::OnDashCompleted()
{
    // End the ability when the dash completes
    K2_EndAbility();
}