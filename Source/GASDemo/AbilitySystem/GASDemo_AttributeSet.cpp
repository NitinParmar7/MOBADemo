// Fill out your copyright notice in the Description page of Project Settings.


#include "GASDemo_AttributeSet.h"

#include "GameplayEffectExtension.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GASDemo/GASDemoCharacter.h"
#include "Net/UnrealNetwork.h"

UGASDemo_AttributeSet::UGASDemo_AttributeSet()
{
}

void UGASDemo_AttributeSet::PostInitProperties()
{
	Super::PostInitProperties();
	MaxHealth.SetBaseValue(100.0f);
    Health.SetBaseValue(MaxHealth.GetBaseValue());
    
    MaxMana.SetBaseValue(100.0f);
    Mana.SetBaseValue(MaxMana.GetBaseValue());
    
    MaxStamina.SetBaseValue(100.0f);
    Stamina.SetBaseValue(MaxStamina.GetBaseValue());
    
    AttackPower.SetBaseValue(10.0f);
    Defense.SetBaseValue(5.0f);
    MoveSpeed.SetBaseValue(600.0f);
}

void UGASDemo_AttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGASDemo_AttributeSet, Health);
	DOREPLIFETIME(UGASDemo_AttributeSet, MaxHealth);
	DOREPLIFETIME(UGASDemo_AttributeSet, Mana);
	DOREPLIFETIME(UGASDemo_AttributeSet, MaxMana);
	DOREPLIFETIME(UGASDemo_AttributeSet, Stamina);
	DOREPLIFETIME(UGASDemo_AttributeSet, MaxStamina);
	DOREPLIFETIME(UGASDemo_AttributeSet, AttackPower);
	DOREPLIFETIME(UGASDemo_AttributeSet, Defense);
	DOREPLIFETIME(UGASDemo_AttributeSet, MoveSpeed);
}

void UGASDemo_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
    if (Attribute == GetMaxHealthAttribute())
    {
        NewValue = FMath::Max(NewValue, 1.0f);
    }
    else if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetMaxManaAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);
    }
    else if (Attribute == GetManaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
    }
    else if (Attribute == GetMaxStaminaAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);
    }
    else if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
    }
}

void UGASDemo_AttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	 // Cache ability system component and owner from effect data
    FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
    UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
    
    // Get target actor
    AActor* TargetActor = nullptr;
    AController* TargetController = nullptr;
    AGASDemoCharacter* TargetCharacter = nullptr;
    
    if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
        TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
        TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
        TargetCharacter = Cast<AGASDemoCharacter>(TargetActor);
    }
    
    // Get source actor
    AActor* SourceActor = nullptr;
    AController* SourceController = nullptr;
    AGASDemoCharacter* SourceCharacter = nullptr;
    
    if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
    {
        SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
        SourceController = Source->AbilityActorInfo->PlayerController.Get();
        SourceCharacter = Cast<AGASDemoCharacter>(SourceActor);
    }
    
    // Get modified attribute
    const FGameplayAttribute ModifiedAttribute = Data.EvaluatedData.Attribute;
    
    // Handle each attribute post modification
    
    // Health
    if (ModifiedAttribute == GetHealthAttribute())
    {
        // Clamp health
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
        
        // Check for death
        if (GetHealth() <= 0.0f && TargetCharacter)
        {
            TargetCharacter->Die();
        }
    }
    // Mana
    else if (ModifiedAttribute == GetManaAttribute())
    {
        // Clamp mana
        SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
    }
    // Stamina
    else if (ModifiedAttribute == GetStaminaAttribute())
    {
        // Clamp stamina
        SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
    }
    // Max Health
    else if (ModifiedAttribute == GetMaxHealthAttribute())
    {
        // Update current health if max health changed
        AdjustAttributeForMaxChange(Health, MaxHealth, GetMaxHealth(), GetHealthAttribute());
    }
    // Max Mana
    else if (ModifiedAttribute == GetMaxManaAttribute())
    {
        // Update current mana if max mana changed
        AdjustAttributeForMaxChange(Mana, MaxMana, GetMaxMana(), GetManaAttribute());
    }
    // Max Stamina
    else if (ModifiedAttribute == GetMaxStaminaAttribute())
    {
        // Update current stamina if max stamina changed
        AdjustAttributeForMaxChange(Stamina, MaxStamina, GetMaxStamina(), GetStaminaAttribute());
    }
    // Movement speed
    else if (ModifiedAttribute == GetMoveSpeedAttribute())
    {
        // Update character movement component
        if (TargetCharacter)
        {
            TargetCharacter->GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
        }
    }
}

void UGASDemo_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASDemo_AttributeSet, Health, OldHealth);
}

void UGASDemo_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASDemo_AttributeSet, MaxHealth, OldMaxHealth);
}

void UGASDemo_AttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASDemo_AttributeSet, Mana, OldMana);
}

void UGASDemo_AttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASDemo_AttributeSet, MaxMana, OldMaxMana);
}

void UGASDemo_AttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASDemo_AttributeSet, Stamina, OldStamina);
}

void UGASDemo_AttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASDemo_AttributeSet, MaxStamina, OldMaxStamina);
}

void UGASDemo_AttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASDemo_AttributeSet, AttackPower, OldAttackPower);
}

void UGASDemo_AttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASDemo_AttributeSet, Defense, OldDefense);
}

void UGASDemo_AttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UGASDemo_AttributeSet, MoveSpeed, OldMoveSpeed);
}

void UGASDemo_AttributeSet::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
                                                        const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
    UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
    if (!AbilityComp)
    {
        return;
    }
    
    // Get current values
    float CurrentValue = AffectedAttribute.GetCurrentValue();
    float CurrentMaxValue = MaxAttribute.GetCurrentValue();
    
    if (CurrentMaxValue > 0.f)
    {
        // Calculate new value proportionally
        float NewDelta = ((CurrentValue / CurrentMaxValue) * NewMaxValue) - CurrentValue;
        AbilityComp->ApplyModToAttribute(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
    }
}
