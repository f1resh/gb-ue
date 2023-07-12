// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePawn.h"

// Sets default values
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	HealthComponent->OnDie.AddUObject(this, &ABasePawn::Die);
	HealthComponent->OnDamaged.AddUObject(this, &ABasePawn::DamageTaked);

}

// Called when the game starts or when spawned
void ABasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABasePawn::Die()
{
	if (Cannon)
		delete Cannon;

	DeathVisualEffect->ActivateSystem();
	//DeathAudioEffect->Play();

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	UE_LOG(LogTemp, Display, TEXT("Actor is ready to die"));

	FTimerHandle destroyHandler;
	GetWorld()->GetTimerManager().SetTimer(destroyHandler, this, &ABasePawn::D, 3, false);

}

void ABasePawn::D() {
	Destroy();
}

void ABasePawn::DamageTaked(float DamageValue)
{
	UE_LOG(LogTemp, Warning, TEXT("BasePawn %s took damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
}

void ABasePawn::Fire()
{
	if (Cannon)
	{
		(*Cannon)->Fire();
	}
}

void ABasePawn::FireSpecial()
{
	if (Cannon) {
		(*Cannon)->FireSpecial();
	}
}

void ABasePawn::TakeDamage(FDamageData DamageData)
{
	HealthComponent->TakeDamage(DamageData);
}
