// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GASDemoCharacter.generated.h"

class UGameplayEffect;
class UGASDemo_AbilitySystemComponent;
class UGASDemo_AttributeSet;
class UGameplayAbility;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class GASDEMO_API AGASDemoCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PrimaryAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SecondaryAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

public:
	AGASDemoCharacter();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Getters for character attributes 
	UFUNCTION(BlueprintCallable, Category = "ARPG|Attributes")
	float GetHealth() const;
    
	UFUNCTION(BlueprintCallable, Category = "ARPG|Attributes")
	float GetMaxHealth() const;
    
	UFUNCTION(BlueprintCallable, Category = "ARPG|Attributes")
	float GetMana() const;
    
	UFUNCTION(BlueprintCallable, Category = "ARPG|Attributes")
	float GetMaxMana() const;
    
	UFUNCTION(BlueprintCallable, Category = "ARPG|Attributes")
	float GetStamina() const;
    
	UFUNCTION(BlueprintCallable, Category = "ARPG|Attributes")
	float GetMaxStamina() const;
    
	UFUNCTION(BlueprintCallable, Category = "ARPG|Attributes")
	float GetAttackPower() const;

	// Death handling
	UFUNCTION(BlueprintCallable, Category = "ARPG|Character")
	virtual bool IsAlive() const;
    
	UFUNCTION(BlueprintCallable, Category = "ARPG|Character")
	virtual void Die();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "ARPG|Character")
	void OnDeath();
    
	// Ability activation
	UFUNCTION(BlueprintCallable, Category = "ARPG|Abilities")
	bool ActivateAbilityByTag(const FGameplayTagContainer& TagContainer);
    
	// Manually trigger ability from animation
	UFUNCTION(BlueprintCallable, Category = "ARPG|Abilities")
	void OnAbilityAnimNotify(FName NotifyName);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Initialize abilities
    virtual void InitializeAbilities();
    
    // Ability granting on spawn
    virtual void GrantDefaultAbilities();
    
    // Apply startup effects (base stats)
    virtual void ApplyStartupEffects();
    
    // Input handling for ability activation
    void OnPrimaryAttackInputPressed();
    void OnSecondaryAttackInputPressed();
    void OnDashInputPressed();

	// Ability System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ARPG|Abilities")
	UGASDemo_AbilitySystemComponent* AbilitySystemComponent;
    
	// Attribute Set
	UPROPERTY()
	UGASDemo_AttributeSet* AttributeSet;
    
	// Default abilities granted to character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARPG|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
    
	// Effects applied on character initialization
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARPG|Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;
    
	// Level for granting abilities
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARPG|Abilities")
	int32 AbilityLevel = 1;
    
	// Is character dead
	UPROPERTY(ReplicatedUsing=OnRep_IsDead,VisibleAnywhere, BlueprintReadOnly, Category = "ARPG|Character")
	bool bIsDead = false;
    
	// Gameplay tags for ability activation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARPG|Abilities")
	FGameplayTag PrimaryAttackTag;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARPG|Abilities")
	FGameplayTag SecondaryAttackTag;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ARPG|Abilities")
	FGameplayTag DashTag;

	UFUNCTION()
	void OnRep_IsDead();

protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

