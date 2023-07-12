// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Scorable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UScorable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TANKOGEDDON_API IScorable
{
	GENERATED_BODY()

	DECLARE_EVENT_OneParam(IScorable, FGetScoreOnDie, int)
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	FGetScoreOnDie GetScoreOnDie;
	virtual int GivePoints() = 0;
};
