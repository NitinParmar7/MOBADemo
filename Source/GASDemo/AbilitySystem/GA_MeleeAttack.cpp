// GA_MeleeAttack.cpp
#include "GA_MeleeAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
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

void UGA_MeleeAttack::OnMontageCompleted()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_MeleeAttack::OnMontageBlendOut()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_MeleeAttack::OnMontageCancelled()
{
    CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_MeleeAttack::OnMontageInterrupted()
{
    CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

void UGA_MeleeAttack::OnMeleeHitEventReceived(FGameplayEventData Payload)
{
    PerformMeleeAttack();
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
    if (UAnimMontage* TempAnimation = AttackAnimations.Num() ? AttackAnimations[FMath::RandRange(0, AttackAnimations.Num() - 1)] : AbilityMontage)
    {
       UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this,
            NAME_None,
            TempAnimation,
            1.0f,
            NAME_None,
            true,
            true
        );
        
        if (Task)
        {
            Task->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnMontageCompleted);
            Task->OnBlendOut.AddDynamic(this, &UGA_MeleeAttack::OnMontageBlendOut);
            Task->OnCancelled.AddDynamic(this, &UGA_MeleeAttack::OnMontageCancelled);
            Task->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnMontageInterrupted);
            Task->ReadyForActivation();

            UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
                this,
                FGameplayTag::RequestGameplayTag(FName("Ability.AnimNotify.MeleeHit")),
                nullptr,
                true,
                true
            );

            WaitEventTask->EventReceived.AddDynamic(this, &UGA_MeleeAttack::OnMeleeHitEventReceived);
            WaitEventTask->ReadyForActivation();
        }
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