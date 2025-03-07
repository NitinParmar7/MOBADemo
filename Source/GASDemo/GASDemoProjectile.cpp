// Fill out your copyright notice in the Description page of Project Settings.


#include "GASDemoProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/StaticMeshComponent.h"


class UAbilitySystemComponent;
// Sets default values
AGASDemoProjectile::AGASDemoProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(15.0f);
	CollisionComponent->SetCollisionProfileName("Projectile");
	CollisionComponent->OnComponentHit.AddDynamic(this, &AGASDemoProjectile::OnHit);
	RootComponent = CollisionComponent;
    
	// Create mesh component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName("NoCollision");
    
	// Create particle component
	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	ParticleComponent->SetupAttachment(RootComponent);
    
	// Create audio component
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
    
	// Create projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = 1500.0f;
	ProjectileMovement->MaxSpeed = 1500.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
    
	// Set default values
	BaseDamage = 15.0f;
    
	// Set default hit effect tag
	HitEffectTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Projectile.Hit"));
    
	// Set replication
	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AGASDemoProjectile::BeginPlay()
{
	Super::BeginPlay();
	// Play audio if available
    if (AudioComponent)
    {
        AudioComponent->Play();
    }
	
}

// Called every frame
void AGASDemoProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGASDemoProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}
    
	// Apply damage to target
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (TargetASC && DamageEffectClass)
	{
		// Get source ability system component
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		if (SourceASC)
		{
			// Create effect context
			FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);
            
			// Create effect spec
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
				DamageEffectClass,
				1,
				EffectContext
			);
            
			if (SpecHandle.IsValid())
			{
				// Set damage
				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FGameplayTag::RequestGameplayTag(FName("Ability.Damage")), BaseDamage);
                
				// Apply effect
				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
	}
    
	// Play hit effects
	if (HitEffectTag.IsValid())
	{
		// Get source ability system component
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		if (SourceASC)
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = Hit.Location;
			CueParams.Normal = Hit.Normal;
			CueParams.PhysicalMaterial = Hit.PhysMaterial;
			CueParams.SourceObject = this;
            
			SourceASC->ExecuteGameplayCue(HitEffectTag, CueParams);
		}
	}
    
	// Destroy projectile
	Destroy();
}

void AGASDemoProjectile::InitializeProjectile(float InDamage)
{
	BaseDamage = InDamage;
}

