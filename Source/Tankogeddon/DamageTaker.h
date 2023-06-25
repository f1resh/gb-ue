// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameStructs.h"
#include "DamageTaker.generated.h"

UINTERFACE(MinimalAPI)
class UDamageTaker : public UInterface
{
	GENERATED_BODY()
};

class TANKOGEDDON_API IDamageTaker
{
	GENERATED_BODY()

public:

    virtual void TakeDamage(FDamageData DamageData) = 0;
};
