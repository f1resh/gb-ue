// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerController.h"
#include "DrawDebugHelpers.h"
#include "TankPawn.h"
#include "Engine/Engine.h"


ATankPlayerController::ATankPlayerController()
{
	bShowMouseCursor = true;
}


void ATankPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ATankPlayerController::MoveForward);
	InputComponent->BindAxis("RotateRight", this, &ATankPlayerController::RotateRight);
	InputComponent->BindAction("Fire", IE_Pressed, this, &ATankPlayerController::Fire);
	InputComponent->BindAxis("TurretRotationRight", this, &ATankPlayerController::RotateTurretRight);
	
}

void ATankPlayerController::Tick(float DeltaTime)
{
	FVector mouseDirection;
	DeprojectMousePositionToWorld(MousePos, mouseDirection);
	FVector pawnPos = TankPawn->GetActorLocation();
	MousePos.Z = pawnPos.Z;
	FVector dir = MousePos - pawnPos;
	dir.Normalize();
	MousePos = pawnPos + dir * 1000;
	//DrawDebugLine(GetWorld(), pawnPos, MousePos, FColor::Green, false, 0.1f, 0, 5);
}

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TankPawn = Cast<ATankPawn>(GetPawn());
}

void ATankPlayerController::MoveForward(float AxisValue)
{

	TankPawn->MoveForward(AxisValue);
}

void ATankPlayerController::RotateRight(float AxisValue)
{
	TankPawn->RotateRight(AxisValue);
}

void ATankPlayerController::RotateTurretRight(float AxisValue)
{
	GEngine->AddOnScreenDebugMessage(5,1,FColor::Blue, FString::SanitizeFloat(AxisValue));
}

void ATankPlayerController::Fire()
{
	TankPawn->Fire();
}

