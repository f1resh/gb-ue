// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStructs.h"
#include "Projectile.h"
#include "MatineeCameraShake.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "Particles/ParticleSystemComponent.h"

#include "Cannon.generated.h"


class UArrowComponent;

UCLASS()
class TANKOGEDDON_API ACannon : public AActor
{
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(ACannon, FOnGetScore, int)
protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
    UArrowComponent * ProjectileSpawnPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UParticleSystemComponent* ShootEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UAudioComponent* AudioEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UForceFeedbackEffect * ShootForceEffect;
    
    UPROPERTY(EditAnywhere)
    TSubclassOf<UMatineeCameraShake> ShootShake;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
    float FireRate = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
	float BurstFireRate = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
	float BurstFireNumber = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
	int RoundsNumber = 20;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
    float FireRange = 1000;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
    float FireDamage = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
    ECannonType Type = ECannonType::FireProjectile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fire params")
    TSubclassOf<AProjectile> ProjectileClass;

	FTimerHandle ReloadTimerHandle;
	FTimerHandle BurstTimerHandle;

	bool ReadyToFire = false;

public:	
    ACannon();

	void Fire();
	void FireSpecial();

	void SwitchType();

	void AddRounds(int);

	bool IsReadyToFire();

	FOnGetScore OnGetScore;
protected:
    virtual void BeginPlay() override;

	void Reload();	

private:
	void SpawnProjectile();
	void SpawnBurst();

	int burstCount = 0;

	void SendScore(int);

};
