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
	FTimerHandle _switchTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(_switchTimerHandle, this, &ATurret::SwitchFireMode, SwitchFireModeTimer, true, SwitchFireModeTimer);

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
		if (BallisticTargeting) ElevateCannon();
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

void ATurret::ElevateCannon()
{
	//demo
	float grav = 9.81f;
	float ProjectileSpeed = 100;
	FRotator currRotation = TurretMesh->GetComponentRotation();
	float Distance = (PlayerPawn->GetActorLocation() - TurretMesh->GetComponentLocation()).Size();
	float sin2A = grav * Distance / ProjectileSpeed / ProjectileSpeed;
	sin2A = sin2A >= 1 ? 1 : sin2A;
	float elevAngle = FMath::RadiansToDegrees(asinf(sin2A)) / 2;
	FRotator targetRotation(elevAngle, currRotation.Roll, currRotation.Yaw);
	TurretMesh->SetWorldRotation(FMath::Lerp(currRotation, targetRotation, TargetingSpeed));
	UE_LOG(LogTemp, Warning, TEXT("Turret %s cannon elevation:%f"), *GetName(), currRotation.Pitch);
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
	
	FVector turretForwardVector = TurretMesh->GetForwardVector();
	FVector targetingDir(turretForwardVector.X, turretForwardVector.Y, 0);
	FVector dirToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();

	dirToPlayer.Normalize();
	targetingDir.Normalize();
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
	if (CannonPtr)
		CannonPtr->Destroy();

	ABasePawn::Die();
}

void ATurret::DamageTaked(float DamageValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Turret %s took damage:%f Health:%f"), *GetName(), DamageValue, HealthComponent->GetHealth());
}

void ATurret::SwitchFireMode()
{
	CannonPtr->SwitchType();
}

FVector ATurret::GetEyesPosition()
{
	return CannonSetupPoint->GetComponentLocation();
}



