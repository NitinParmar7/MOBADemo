// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASDemoCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AbilitySystem/GASDemo_AbilitySystemComponent.h"
#include "AbilitySystem/GASDemo_AttributeSet.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGASDemoCharacter

AGASDemoCharacter::AGASDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	AbilitySystemComponent = CreateDefaultSubobject<UGASDemo_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UGASDemo_AttributeSet>(TEXT("AttributeSet"));

	PrimaryAttackTag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Melee"));
	SecondaryAttackTag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Ranged"));
	DashTag = FGameplayTag::RequestGameplayTag(FName("Ability.Movement.Dash"));

	bReplicates = true;
	SetReplicateMovement(true);
}

void AGASDemoCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (GetLocalRole() == ROLE_Authority)
		{
			InitializeAbilities();
		}
	}
}

void AGASDemoCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGASDemoCharacter, bIsDead);
}

UAbilitySystemComponent* AGASDemoCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float AGASDemoCharacter::GetHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetHealth();
	}

	return 0.0f;
}

float AGASDemoCharacter::GetMaxHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxHealth();
	}
	return 0.0f;
}

float AGASDemoCharacter::GetMana() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMana();
	}
	return 0.0f;
}

float AGASDemoCharacter::GetMaxMana() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxMana();
	}
	return 0.0f;
}

float AGASDemoCharacter::GetStamina() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetStamina();
	}
	return 0.0f;
}

float AGASDemoCharacter::GetMaxStamina() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxStamina();
	}
	return 0.0f;
}

float AGASDemoCharacter::GetAttackPower() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetAttackPower();
	}
	return 0.0f;
}

bool AGASDemoCharacter::IsAlive() const
{
	return !bIsDead;
}

void AGASDemoCharacter::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}

	OnDeath();
	DetachFromControllerPendingDestroy();
}

bool AGASDemoCharacter::ActivateAbilityByTag(const FGameplayTagContainer& TagContainer)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
	}
	return false;
}

void AGASDemoCharacter::OnAbilityAnimNotify(FName NotifyName)
{
}

//////////////////////////////////////////////////////////////////////////
// Input


void AGASDemoCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGASDemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGASDemoCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGASDemoCharacter::Look);

		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Triggered, this, &AGASDemoCharacter::OnPrimaryAttackInputPressed);
		EnhancedInputComponent->BindAction(SecondaryAttackAction, ETriggerEvent::Triggered, this, &AGASDemoCharacter::OnSecondaryAttackInputPressed);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AGASDemoCharacter::OnDashInputPressed);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGASDemoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGASDemoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGASDemoCharacter::InitializeAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || !AbilitySystemComponent->bCharacterAbilitiesInitialized)
	{
		return;
	}

	GrantDefaultAbilities();

	ApplyStartupEffects();

	AbilitySystemComponent->bCharacterAbilitiesInitialized = true;
}

void AGASDemoCharacter::GrantDefaultAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent)
	{
		return;
	}

	for (TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
	{
		if (StartupAbility)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
		}
	}
}

void AGASDemoCharacter::ApplyStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect>& StartupEffect : StartupEffects)
	{
		if (StartupEffect)
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(StartupEffect, AbilityLevel, EffectContext);
			if (SpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}

void AGASDemoCharacter::OnPrimaryAttackInputPressed()
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(PrimaryAttackTag);
	ActivateAbilityByTag(TagContainer);
}

void AGASDemoCharacter::OnSecondaryAttackInputPressed()
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(SecondaryAttackTag);
	ActivateAbilityByTag(TagContainer);
}

void AGASDemoCharacter::OnDashInputPressed()
{
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(DashTag);
	ActivateAbilityByTag(TagContainer);
}

void AGASDemoCharacter::OnRep_IsDead()
{
	
}





