// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "DamageTaker.h"
#include "Scorable.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent * sceeneCpm = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = sceeneCpm;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnMeshOverlapBegin);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
}

void AProjectile::Start()
{
	GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &AProjectile::Move, MoveRate, true, MoveRate);
}

void AProjectile::OnMeshOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile %s collided with %s. "), *GetName(), *OtherActor->GetName());
	AActor* owner = GetOwner();
	AActor* ownerByOwner = owner != nullptr? owner->GetOwner(): nullptr;
	if(OtherActor != owner && OtherActor != ownerByOwner)
	{
		IDamageTaker * damageTakerActor = Cast<IDamageTaker>(OtherActor);
		if(damageTakerActor)
		{
			FDamageData damageData;
			damageData.DamageValue = Damage;
			damageData.Instigator = owner;
			damageData.DamageMaker = this;

			IScorable* scorable = Cast<IScorable>(OtherActor);
			if (scorable) {
				scorable->GetScoreOnDie.AddUObject(this, &AProjectile::GiveScore);
			}
			damageTakerActor->TakeDamage(damageData);

		}
		else
		{
			//OtherActor->Destroy();
			
			UPrimitiveComponent* mesh = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
			if(mesh)
			{
				if(mesh->IsSimulatingPhysics())
				{
					UE_LOG(LogTemp, Warning, TEXT("Simulate physics enabled %s. "), *OtherActor->GetName());
					FVector forceVector =   OtherActor->GetActorLocation() - GetActorLocation();
					forceVector.Normalize();
					//mesh->AddForce(forceVector * PushForce);
					mesh->AddImpulse(forceVector * PushForce, NAME_None, true);
				}
			}
		}
		
		Destroy();
		//UE_LOG(LogTemp, Warning, TEXT("Projectile destroyed"));
	}	
	// OtherActor->Destroy();
	// Destroy();
}

void AProjectile::Move()
{
	FVector nextPosition = GetActorLocation() + GetActorForwardVector() * MoveSpeed * MoveRate;
	SetActorLocation(nextPosition);
}

void AProjectile::GiveScore(int score)
{
	if (OnGetScore.IsBound()) {
		OnGetScore.Broadcast(score);
	}
}



