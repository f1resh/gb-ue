// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"



// Sets default values
ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret body"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret turret"));
	TurretMesh->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform);

	CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
	CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

	HitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit collider"));
	HitCollider->SetupAttachment(BodyMesh);

	// UStaticMesh * turretMeshTemp = LoadObject<UStaticMesh>(this, *TurretMeshPath);
	// if(turretMeshTemp)
	// 	TurretMesh->SetStaticMesh(turretMeshTemp);
	//
	// UStaticMesh * bodyMeshTemp = LoadObject<UStaticMesh>(this, *BodyMeshPath);
	// if(bodyMeshTemp)
	// 	BodyMesh->SetStaticMesh(bodyMeshTemp);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters params;
	params.Owner = this;
	CannonPtr = GetWorld()->SpawnActor<ACannon>(CannonClass, params);
	CannonPtr->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Cannon = &CannonPtr;


	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	
	FTimerHandle _targetingTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(_targetingTimerHandle, this, &ATurret::Targeting, TargetingRate, true, TargetingRate);

	UStaticMesh * turretMeshTemp = LoadObject<UStaticMesh>(this, *TurretMeshPath);
	if(turretMeshTemp)
		TurretMesh->SetStaticMesh(turretMeshTemp);
	
	UStaticMesh * bodyMeshTemp = LoadObject<UStaticMesh>(this, *BodyMeshPath);
	if(bodyMeshTemp)
		BodyMesh->SetStaticMesh(bodyMeshTemp);

}

void ATurret::Destroyed()
{
	if(CannonPtr)
		CannonPtr->Destroy();
}

void ATurret::Targeting()
{
	// 
	if(IsPlayerInRange() && IsPlayerSeen())
	{
		RotateToPlayer();
	}

	if(CanFire() && CannonPtr && CannonPtr->IsReadyToFire() && IsPlayerInRange() && IsPlayerSeen())
	{
		Fire();
	}
}

void ATurret::RotateToPlayer()
{
	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation());
	FRotator currRotation = TurretMesh->GetComponentRotation();
	targetRotation.Pitch = currRotation.Pitch;
	targetRotation.Roll = currRotation.Roll;
	TurretMesh->SetWorldRotation(FMath::Lerp(currRotation, targetRotation, TargetingSpeed));
}

bool ATurret::IsPlayerInRange()
{
	if(!PlayerPawn)
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if(!PlayerPawn)
		return false;
	
	return FVector::Distance(PlayerPawn->GetActorLocation(), GetActorLocation()) <= TargetingRange;
}

bool ATurret::IsPlayerSeen()
{

	if (!PlayerPawn)
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (!PlayerPawn)
		return false;

	FVector playerPos = PlayerPawn->GetActorLocation();
	FVector eyesPos = GetEyesPosition();

	FHitResult hitResult;
	FCollisionQueryParams traceParams =
		FCollisionQueryParams(FName(TEXT("FireTrace")), true, this);

	traceParams.bTraceComplex = true;
	traceParams.AddIgnoredActor(this);
	traceParams.AddIgnoredActor(*Cannon);
	traceParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->LineTraceSingleByChannel(hitResult, eyesPos, playerPos,
		ECollisionChannel::ECC_Visibility, traceParams))
	{
		if (hitResult.GetActor())
		{
			return hitResult.GetActor() == PlayerPawn;
		}
	}
	return false;

}

bool ATurret::CanFire()
{
	if(!PlayerPawn)
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if(!PlayerPawn)
		return false;
	
	FVector targetingDir = TurretMesh->GetForwardVector();
	FVector dirToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();
	dirToPlayer.Normalize();
	float aimAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(targetingDir, dirToPlayer)));
	return aimAngle <= Accurency;
}

int ATurret::GivePoints()
{
	return Points;
}

void ATurret::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// UStaticMesh * turretMeshTemp = LoadObject<UStaticMesh>(this, *TurretMeshPath);
	// if(turretMeshTemp)
	// 	TurretMesh->SetStaticMesh(turretMeshTemp);
	//
	// UStaticMesh * bodyMeshTemp = LoadObject<UStaticMesh>(this, *BodyMeshPath);
	// if(bodyMeshTemp)
	// 	BodyMesh->SetStaticMesh(bodyMeshTemp);
}

void ATurret::Die()
{
	if (GetScoreOnDie.IsBound())
		GetScoreOnDie.Broadcast(GivePoints());
	Destroy();
}

void ATurret::DamageTaked(float DamageValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Turret %s took damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
}

FVector ATurret::GetEyesPosition()
{
	return CannonSetupPoint->GetComponentLocation();
}



