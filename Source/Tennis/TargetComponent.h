// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "TargetComponent.generated.h"

/**
 * 
 */
UCLASS()
class TENNIS_API UTargetComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UTargetComponent();
	

private:
	UPROPERTY(EditAnywhere, Category = "Sprite")
		UStaticMeshComponent *tgtMesh;
};
