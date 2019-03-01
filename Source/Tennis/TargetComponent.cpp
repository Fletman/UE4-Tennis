// Fill out your copyright notice in the Description page of Project Settings.

#include "TargetComponent.h"
#include "Components/StaticMeshComponent.h"

UTargetComponent::UTargetComponent()
{
	tgtMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target Sprite"));
}