// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTaker.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class TANKOGEDDON_API AProjectile : public AActor
{
	GENERATED_BODY()
	
	DECLARE_EVENT_OneParam(AProjectile, FOnGetScore, int)
protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
    UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
    float MoveSpeed = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
    float MoveRate = 0.005f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
    float Damage = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	float PushForce = 100000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explode")
	bool EnableExplode = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explode")
	float ExplodeRadius = 200;

	FTimerHandle MovementTimerHandle;
	
public:	
    AProjectile();

	virtual void Start();

	FOnGetScore OnGetScore;

protected:
    UFUNCTION()
    void OnMeshOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    virtual void Move();

	UFUNCTION()
	virtual void Explode();

	void GiveScore(int);

	void InflictDamage(IDamageTaker*);
	void PushObject(AActor*);
};
