// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "GASDemoProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UGameplayEffect;
class UParticleSystemComponent;
class UAudioComponent;

UCLASS()
class GASDEMO_API AGASDemoProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGASDemoProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Collision component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;
    
	// Projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;
    
	// Projectile mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;
    
	// Projectile particle effect
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ParticleComponent;
    
	// Projectile audio
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComponent;
    
	// The damage effect to apply
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
    
	// Base damage amount
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float BaseDamage = 15.0f;
    
	// Gameplay tag for hit effect
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGameplayTag HitEffectTag;
    
	// Function called when projectile hits something
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
    
	// Function to initialize projectile
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void InitializeProjectile(float InDamage);
};
