// Fill out your copyright notice in the Description page of Project Settings.

#include "AIControl.h"
#include "TennisPlayer.h"
#include "Engine.h"


void AAIControl::BeginPlay()
{
	Super::BeginPlay();

	AIChar = Cast<ATennisPlayer>(GetPawn());
	if (!AIChar)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7, FColor::Red, FString::Printf(TEXT("No AI Pawn Found")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("GOT EM"));

		AIChar->forwardInput = 0;
		AIChar->sideInput = 0;
	}
	
}

void AAIControl::Tick(float DeltaTime)
{
	if(AIChar->Ball->GetMovement()->Velocity.X <= 0)
	{
		AIChar->SetCharState(RALLY);
		ReturnToCenter();
		return;
	}

	
	FVector locDiff = AIChar->GetActorLocation() - AIChar->Ball->GetActorLocation();
	if (FMath::Abs<float>(locDiff.X) <= 100 && FMath::Abs<float>(locDiff.Y) <= 100 && FMath::Abs<float>(locDiff.Z <= 100))
	{
		AIChar->aimX = FMath::RandRange(-680, 680);
		AIChar->Swing(TOPSPIN_SWING);
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Silver, TEXT("Swing!"));
	}
	else
	{
		AIChar->SetPrep();
	}
}

void AAIControl::ReturnToCenter()
{
	if (AIChar->GetActorLocation().Y < -10)
	{
		AIChar->sideInput = -1;
	}

	else if (AIChar->GetActorLocation().Y > 10)
	{
		AIChar->sideInput = 1;
	}

	else
	{
		AIChar->sideInput = 0;
	}

	if (AIChar->GetActorLocation().X > 1845)
	{
		AIChar->forwardInput = 1;
	}

	else
	{
		AIChar->forwardInput = 0;
	}
}