// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

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
			InflictDamage(damageTakerActor);
		}
		else
		{
			PushObject(OtherActor);
		}
		if (EnableExplode) Explode();
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

void AProjectile::Explode()
{
	if (!EnableExplode) return;

	FVector startPos = GetActorLocation();
	FVector endPos = startPos + FVector(0.1f);

	FCollisionShape Shape = FCollisionShape::MakeSphere(ExplodeRadius);
	FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
	params.AddIgnoredActor(this);
	params.bTraceComplex = true;
	params.TraceTag = "Explode Trace";
	TArray<FHitResult> AttackHit;

	FQuat Rotation = FQuat::Identity;

	bool sweepResult = GetWorld()->SweepMultiByChannel
	(
		AttackHit,
		startPos,
		endPos,
		Rotation,
		ECollisionChannel::ECC_Visibility,
		Shape,
		params
	);

	GetWorld()->DebugDrawTraceTag = "Explode Trace";

	if (sweepResult)
	{
		for (FHitResult hitResult : AttackHit)
		{
			AActor* otherActor = hitResult.GetActor();
			if (!otherActor)
				continue;

			IDamageTaker* damageTakerActor = Cast<IDamageTaker>(otherActor);
			if (damageTakerActor)
			{
				InflictDamage(damageTakerActor);
			}
			else
			{
				PushObject(otherActor);
			}

		}
	}
}


void AProjectile::InflictDamage(IDamageTaker* damageTakerActor)
{
	FDamageData damageData;
	damageData.DamageValue = Damage;
	damageData.Instigator = GetOwner();
	damageData.DamageMaker = this;
	
	IScorable* scorable = Cast<IScorable>(damageTakerActor);
	if (scorable) {
		scorable->GetScoreOnDie.AddUObject(this, &AProjectile::GiveScore);
	}

	damageTakerActor->TakeDamage(damageData);
}

void AProjectile::PushObject(AActor* otherActor)
{
	UPrimitiveComponent* mesh = Cast<UPrimitiveComponent>(otherActor->GetRootComponent());
	if (mesh)
	{
		if (mesh->IsSimulatingPhysics())
		{
			FVector forceVector = otherActor->GetActorLocation() - GetActorLocation();
			forceVector.Normalize();
			mesh->AddForce(forceVector * PushForce, NAME_None, false);
		}
	}
}
