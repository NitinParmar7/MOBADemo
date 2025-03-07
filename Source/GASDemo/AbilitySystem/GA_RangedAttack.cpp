// GA_RangedAttack.cpp
#include "GA_RangedAttack.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GASDemo/GASDemoCharacter.h"
#include "Kismet/GameplayStatics.h"

UGA_RangedAttack::UGA_RangedAttack()
{
    // Default constructor
    
    // Set ability tags
    AbilityInputTag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Ranged"));
    
    // Setup gameplay tags
    FGameplayTagContainer GameplayTagContainer;
    GameplayTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Ranged")));
    GameplayTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack")));
    SetAssetTags(GameplayTagContainer);
    
    // Setup cancelation tags
    FGameplayTagContainer CancelTags;
    CancelTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Movement.Dash")));
    CancelAbilitiesWithTag = CancelTags;
    BlockAbilitiesWithTag = CancelTags;
    
    // Effect gameplay cue
    EffectGameplayCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Attack.Ranged"));
    
    // Set instancing policy
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    
    // Set network policy
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_RangedAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
                                     const FGameplayAbilityActorInfo* ActorInfo, 
                                     const FGameplayAbilityActivationInfo ActivationInfo, 
                                     const FGameplayEventData* TriggerEventData)
{
    // Check mana cost
    AGASDemoCharacter* Character = GetCharacterFromActorInfo();
    if (!Character || Character->GetMana() < ManaCost)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    // Apply cost and commit ability
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    // Apply mana cost
    if (CostEffectClass)
    {
        FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
        EffectContext.AddSourceObject(Character);
        
        FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
            CostEffectClass,
            GetAbilityLevel(),
            EffectContext
        );
        
        if (SpecHandle.IsValid())
        {
            GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
    
    // Play montage
    if (AbilityMontage)
    {
        PlayAbilityMontage(AbilityMontage, 1.0f);
        
        // Note: The actual projectile spawning will happen through an animation notify
        // that calls SpawnProjectile or through direct call in Blueprint
    }
    else
    {
        // No montage, just spawn projectile directly
        SpawnProjectile();
        
        // Apply cooldown
        if (CooldownEffectClass)
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
        
        // End ability immediately
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

void UGA_RangedAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, 
                                const FGameplayAbilityActorInfo* ActorInfo, 
                                const FGameplayAbilityActivationInfo ActivationInfo, 
                                bool bReplicateEndAbility, 
                                bool bWasCancelled)
{
    // Apply cooldown if not cancelled
    if (!bWasCancelled && CooldownEffectClass)
    {
        FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
        EffectContext.AddSourceObject(GetCharacterFromActorInfo());
        
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

void UGA_RangedAttack::SpawnProjectile()
{
    if (!GetCharacterFromActorInfo() || !ProjectileClass)
    {
        return;
    }
    
    // Get character transform
    AGASDemoCharacter* Character = GetCharacterFromActorInfo();
    FVector SpawnLocation = Character->GetActorLocation() + (Character->GetActorForwardVector() * SpawnOffset.X) + 
                          (Character->GetActorRightVector() * SpawnOffset.Y) +
                          (Character->GetActorUpVector() * SpawnOffset.Z);
    
    FRotator SpawnRotation = Character->GetActorRotation();
    
    // Spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Character;
    SpawnParams.Instigator = Character;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    // Spawn projectile
    AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
    
    // Set projectile properties
    // Note: You would typically have a specific projectile class with a component to set damage, etc.
    
    // Trigger gameplay cue
    if (EffectGameplayCueTag.IsValid())
    {
        FGameplayCueParameters CueParams;
        CueParams.Location = SpawnLocation;
        CueParams.Normal = Character->GetActorForwardVector();
        CueParams.SourceObject = Character;
        
        GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(EffectGameplayCueTag, CueParams);
    }
}