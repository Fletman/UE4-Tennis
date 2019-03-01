// Fill out your copyright notice in the Description page of Project Settings.

#include "TennisRacquet.h"
#include "BallTarget.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATennisRacquet::ATennisRacquet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RacquetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(RacquetMesh);
}
