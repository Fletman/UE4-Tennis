// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControl.generated.h"

/**
 * 
 */
UCLASS()
class TENNIS_API AAIControl : public AAIController
{
	GENERATED_BODY()
	
private:
	class ATennisPlayer *AIChar;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	void ReturnToCenter(); //return character to center court, awaiting next ball
};
