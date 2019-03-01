// Fill out your copyright notice in the Description page of Project Settings.

#include "BallTarget.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABallTarget::ABallTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//mesh whose world coordinates are used for targeting
	TargetArea = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target"));
	SetRootComponent(TargetArea);
}
