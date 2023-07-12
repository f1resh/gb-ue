// Fill out your copyright notice in the Description page of Project Settings.


#include "Cannon.h"

#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "DamageTaker.h"
#include "GameStructs.h"
#include "Engine/Engine.h"

ACannon::ACannon()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent * sceeneCpm = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = sceeneCpm;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon mesh"));
	Mesh->SetupAttachment(RootComponent);

	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn point"));
	ProjectileSpawnPoint->SetupAttachment(Mesh);

	ShootEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Shoot effect"));
	ShootEffect->SetupAttachment(ProjectileSpawnPoint);

	AudioEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio effect"));
	ShootEffect->SetupAttachment(ProjectileSpawnPoint);

	SetActorHiddenInGame(true);

}

void ACannon::Fire()
{
	if (RoundsNumber <= 0)
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Unable to fire projectile: Ammo depleted");
		return;
	}
	if (!ReadyToFire)
	{
		return;
	}
	ReadyToFire = false;
	
	ShootEffect->ActivateSystem();
	AudioEffect->Play();
	
	if(GetOwner() && GetOwner() == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		if(ShootForceEffect)
		{
			FForceFeedbackParameters shootForceEffectParams;
			shootForceEffectParams.bLooping = false;
			shootForceEffectParams.Tag = "shootForceEffectParams";
			GetWorld()->GetFirstPlayerController()->ClientPlayForceFeedback(ShootForceEffect, shootForceEffectParams);
		}

		if(ShootShake)
		{
			//GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(ShootShake, 1.0f);
			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(ShootShake);
		}
	}
	
	if(Type == ECannonType::FireProjectile)
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Fire - projectile");
		SpawnProjectile();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(10, 1,FColor::Green, "Fire - trace");
		FHitResult hitResult;
		FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
		traceParams.bTraceComplex = true;
		traceParams.bReturnPhysicalMaterial = false;
		
		FVector start = ProjectileSpawnPoint->GetComponentLocation();
		FVector end = ProjectileSpawnPoint->GetForwardVector() * FireRange + start;
		if(GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility, traceParams))
		{
			DrawDebugLine(GetWorld(), start, hitResult.Location, FColor::Red, false, 0.5f, 0, 5);
			IDamageTaker* damageTakerActor = Cast<IDamageTaker>(hitResult.GetActor());
			if (damageTakerActor)
			{
				FDamageData damageData;
				damageData.DamageValue = FireDamage;
				damageData.Instigator = this;
				damageData.DamageMaker = this;

				damageTakerActor->TakeDamage(damageData);
			}
			else {
				hitResult.GetActor()->Destroy();
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 0.5f, 0, 5);
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1 / FireRate, false);
	RoundsNumber--;
	FString n = this->GetClass()->GetName();
	UE_LOG(LogTemp, Display, TEXT("Number of rounds: %d. Cannon: %s"), RoundsNumber, *n);
}

void ACannon::FireSpecial()
{
	if (RoundsNumber <= 0)
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Unable to fire projectile: Ammo depleted");
		return;
	}
	if (!ReadyToFire)
	{
		return;
	}

	ReadyToFire = false;

	ShootEffect->ActivateSystem();
	AudioEffect->Play();

	if (GetOwner() && GetOwner() == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		if (ShootForceEffect)
		{
			FForceFeedbackParameters shootForceEffectParams;
			shootForceEffectParams.bLooping = false;
			shootForceEffectParams.Tag = "shootForceEffectParams";
			GetWorld()->GetFirstPlayerController()->ClientPlayForceFeedback(ShootForceEffect, shootForceEffectParams);
		}

		if (ShootShake)
		{
			//GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(ShootShake, 1.0f);
			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(ShootShake);
		}
	}

	if (Type == ECannonType::FireProjectile)
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Fire burst - projectile");
		SpawnBurst();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(10, 1, FColor::Green, "Fire - trace secondary");
		FHitResult hitResult;
		FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);
		traceParams.bTraceComplex = true;
		traceParams.bReturnPhysicalMaterial = false;

		FVector start = ProjectileSpawnPoint->GetComponentLocation();
		FVector end = ProjectileSpawnPoint->GetForwardVector() * FireRange * 2 + start;
		if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility, traceParams))
		{
			DrawDebugLine(GetWorld(), start, hitResult.Location, FColor::Green, false, 0.5f, 0, 5);

			IDamageTaker* damageTakerActor = Cast<IDamageTaker>(hitResult.GetActor());
			if (damageTakerActor)
			{
				FDamageData damageData;
				damageData.DamageValue = FireDamage / 2;
				damageData.Instigator = this;
				damageData.DamageMaker = this;

				damageTakerActor->TakeDamage(damageData);
			}
			else {
				hitResult.GetActor()->Destroy();
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 0.5f, 0, 5);
		}
	}
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACannon::Reload, 1 / FireRate, false);
	RoundsNumber--;
	UE_LOG(LogTemp, Display, TEXT("Number of rounds: %d"), RoundsNumber);
}

void ACannon::SwitchType()
{
	Type = static_cast<ECannonType>(static_cast<uint8>(Type) ^ 1);
}

void ACannon::AddRounds(int number)
{
	if (number > 0) RoundsNumber += number;
	UE_LOG(LogTemp, Display, TEXT("Number of rounds: %d"), RoundsNumber);
}

bool ACannon::IsReadyToFire()
{
	return ReadyToFire;
}

void ACannon::Reload()
{
	ReadyToFire = true;
}

void ACannon::SpawnProjectile()
{
	FTransform projectileTransform(ProjectileSpawnPoint->GetComponentRotation(), ProjectileSpawnPoint->GetComponentLocation(), FVector(1));

	//AProjectile* projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, projectileTransform, this);
	AProjectile* projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
	if (projectile)
	{
		// ....
		//projectile->FinishSpawning(projectileTransform);
		projectile->OnGetScore.AddUObject(this, &ACannon::SendScore);
		projectile->Start();
	}
}

void ACannon::SpawnBurst() {
	if (burstCount < BurstFireNumber) {
		SpawnProjectile();
		burstCount++;
		GetWorld()->GetTimerManager().SetTimer(BurstTimerHandle, this, &ACannon::SpawnBurst, 1 / BurstFireRate, false);
	}
	else {
		burstCount = 0;
	}
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	Reload();
}

void ACannon::SendScore(int score)
{
	if (OnGetScore.IsBound())
		OnGetScore.Broadcast(score);
}
