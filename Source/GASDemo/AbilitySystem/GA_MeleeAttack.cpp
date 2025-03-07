// GA_MeleeAttack.cpp
#include "GA_MeleeAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GASDemo/GASDemoCharacter.h"

UGA_MeleeAttack::UGA_MeleeAttack()
{
    // Default constructor
    
    // Set ability tags
    AbilityInputTag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Melee"));
    
    // Setup gameplay tags
    FGameplayTagContainer AbilityTagsContainer;
    AbilityTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Melee")));
    AbilityTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack")));
    SetAssetTags(AbilityTagsContainer);
    
    // Setup cancelation tags
    FGameplayTagContainer CancelTagsContainer;
    CancelTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Movement.Dash")));
    CancelAbilitiesWithTag = CancelTagsContainer;
    BlockAbilitiesWithTag = CancelTagsContainer;
    
    // Effect gameplay cue
    EffectGameplayCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Attack.Melee"));
    
    // Set instancing policy
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    
    // Set network policy
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
                                    const FGameplayAbilityActorInfo* ActorInfo, 
                                    const FGameplayAbilityActivationInfo ActivationInfo, 
                                    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
    
    // Play montage
    if (AbilityMontage)
    {
        PlayAbilityMontage(AbilityMontage, 1.0f);
        
        // Note: The actual damage application will happen through an animation notify
        // that calls PerformMeleeAttack or through direct call in Blueprint
    }
    else
    {
        // No montage, just perform attack directly
        PerformMeleeAttack();
        
        // End ability immediately
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

void UGA_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, 
                               const FGameplayAbilityActorInfo* ActorInfo, 
                               const FGameplayAbilityActivationInfo ActivationInfo, 
                               bool bReplicateEndAbility, 
                               bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeAttack::PerformMeleeAttack()
{
    if (!GetCharacterFromActorInfo())
    {
        return;
    }
    
    // Get character transform
    AGASDemoCharacter* Character = GetCharacterFromActorInfo();
    FVector StartLocation = Character->GetActorLocation();
    FVector ForwardVector = Character->GetActorForwardVector();
    FVector EndLocation = StartLocation + (ForwardVector * AttackRange);
    
    // Perform sphere trace
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Character);
    
    TArray<FHitResult> HitResults;
    bool bHit = UKismetSystemLibrary::SphereTraceMulti(
        GetWorld(),
        StartLocation,
        EndLocation,
        AttackRadius,
        UEngineTypes::ConvertToTraceType(ECC_Camera),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        HitResults,
        true
    );
    
    // Process hits
    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            if (Hit.GetActor() && Hit.GetActor() != Character)
            {
                // Apply damage effect to target
                if (DamageEffectClass)
                {
                    // Get ability system component
                    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor());
                    if (TargetASC)
                    {
                        // Create damage effect spec
                        FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
                        EffectContext.AddSourceObject(Character);
                        
                        FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
                            DamageEffectClass,
                            GetAbilityLevel(),
                            EffectContext
                        );
                        
                        if (SpecHandle.IsValid())
                        {
                            // Add damage to spec
                            float ScaledDamage = BaseDamage;
                            
                            // Could apply multipliers based on character stats here
                            if (Character->GetAttackPower() > 0.0f)
                            {
                                ScaledDamage *= (Character->GetAttackPower() / 10.0f);
                            }
                            
                            // Set the damage on the spec
                            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FGameplayTag::RequestGameplayTag(FName("Ability.Damage")), ScaledDamage);
                            
                            // Apply effect
                            GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
                            
                            // Trigger gameplay cue
                            if (EffectGameplayCueTag.IsValid())
                            {
                                FGameplayCueParameters CueParams;
                                CueParams.Location = Hit.Location;
                                CueParams.Normal = Hit.Normal;
                                CueParams.PhysicalMaterial = Hit.PhysMaterial;
                                CueParams.SourceObject = Character;
                                
                                GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(EffectGameplayCueTag, CueParams);
                            }
                        }
                    }
                }
            }
        }
    }
}