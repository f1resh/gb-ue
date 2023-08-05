// Fill out your copyright notice in the Description page of Project Settings.


#include "TankFactory.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MapLoader.h"

ATankFactory::ATankFactory()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent * sceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = sceneComp;
	
	BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Mesh"));
	BuildingMesh->SetupAttachment(sceneComp);

	DestroyedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Destroyed Building Mesh"));
	DestroyedMesh->SetupAttachment(sceneComp);
	DestroyedMesh->SetHiddenInGame(true);

	DestroyEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Destroy effect"));
	DestroyEffect->SetupAttachment(BuildingMesh);

	DestroyAudioEffect = CreateDefaultSubobject<UAudioComponent>(TEXT("Destroy Audio effect"));
	DestroyAudioEffect->SetupAttachment(BuildingMesh);

	TankSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	TankSpawnPoint->AttachToComponent(sceneComp, FAttachmentTransformRules::KeepRelativeTransform);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(sceneComp);
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnDie.AddUObject(this, &ATankFactory::Die);
	HealthComponent->OnDamaged.AddUObject(this, &ATankFactory::DamageTaked);
}

void ATankFactory::BeginPlay()
{
	Super::BeginPlay();
	if(LinkedMapLoader)
		LinkedMapLoader->SetIsActivated(false);

	GetWorld()->GetTimerManager().SetTimer(_spawnTimerHandle, this, &ATankFactory::SpawnController, SpawnTankRate, true, SpawnTankRate);
}

void ATankFactory::TakeDamage(FDamageData DamageData)
{
	HealthComponent->TakeDamage(DamageData);
}

void ATankFactory::Die()
{
	if(LinkedMapLoader)
		LinkedMapLoader->SetIsActivated(true);
	DestroyEffect->ActivateSystem();
	DestroyAudioEffect->Play();
	BuildingMesh->SetHiddenInGame(true);
	DestroyedMesh->SetHiddenInGame(false);
	//Destroy();
}

void ATankFactory::DamageTaked(float DamageValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Factory %s taked damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
}

void ATankFactory::SpawnNewTank()
{
	FTransform spawnTransform(TankSpawnPoint->GetComponentRotation(), TankSpawnPoint->GetComponentLocation(), FVector(1));
	ATankPawn * newTank = GetWorld()->SpawnActorDeferred<ATankPawn>(SpawnTankClass, spawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	//
	newTank->SetPatrollingPoints(TankWayPoints);
	//
	UGameplayStatics::FinishSpawningActor(newTank, spawnTransform);
}

void ATankFactory::SpawnController()
{
	if (SpawnTankCounter++ < SpawnTankNumber) SpawnNewTank();
	else GetWorldTimerManager().ClearTimer(_spawnTimerHandle);
}


