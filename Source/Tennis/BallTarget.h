// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallTarget.generated.h"

UCLASS()
class TENNIS_API ABallTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABallTarget();

	UPROPERTY(EditAnywhere, Category = "Targeting Area")
		//sphere component used for ball homing
		UStaticMeshComponent *TargetArea;
};
