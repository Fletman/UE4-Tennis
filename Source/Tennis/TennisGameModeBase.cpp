// Fill out your copyright notice in the Description page of Project Settings.

#include "TennisGameModeBase.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"

ATennisGameModeBase::ATennisGameModeBase()
{
	DefaultPawnClass = ATennisPlayer::StaticClass();
	PlayerControllerClass = APlayerControl::StaticClass();
	
}

void ATennisGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	ScoreInit();
	SpawnBall();
	Ball->GetMovement()->Velocity.X = -500;
}

void ATennisGameModeBase::Tick(float DeltaTime)
{

}

void ATennisGameModeBase::GetPlayers()
{
	//TODO: I'm not 100% convinced this is fixed, but I'll leave it for now...

	TArray<AActor*> TempPlayers; //array of player references
	
	Players.SetNum(2);

	ATennisPlayer *tP;

	//find all Tennis Player actors
	//TODO: this will only work for Singles gamemodes, must be overridden for doubles
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATennisPlayer::StaticClass(), TempPlayers);
	for (auto Actor : TempPlayers)
	{
		tP = Cast<ATennisPlayer>(Actor);
		if (tP)
		{
			Players[tP->GetSide()] = tP;
			GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, FString::Printf(TEXT("%d"), tP->GetSide()));
		}
	}
}

//initialize zeroed scoring structs
void ATennisGameModeBase::ScoreInit()
{
	Scoreboard.points[HEADS] = 0; Scoreboard.points[TAILS] = 0;
	Scoreboard.games[HEADS] = 0; Scoreboard.games[TAILS] = 0;
	Scoreboard.tiebreak[HEADS] = 0; Scoreboard.tiebreak[TAILS] = 0;
	Scoreboard.sets[HEADS] = 0; Scoreboard.sets[TAILS] = 0;

	//can't have an even number for max sets
	if(MaxSets % 2 == 0)
	{MaxSets++;}
}

void ATennisGameModeBase::SpawnBall()
{
	Ball = World->SpawnActor<ATennisBall>(gameBall, FVector(-1000, 0, 250), FRotator(0, 0, 0));
}

//check bounds of ball, score point if out-of-bounds (called from TennisBallBP)
void ATennisGameModeBase::BoundsCheck(FVector BallLocation)
{
	FVector SinglesBounds(1790, 665, -1);

	//check if ball is outside specified bounds
	if (FMath::Abs<float>(BallLocation.X) > SinglesBounds.X || FMath::Abs<float>(BallLocation.Y) > SinglesBounds.Y)
	{
		//last side to hit ball would have (presumably) sent it out of bounds
		//award point to opponent of lastToHit
		UpdatePoints((Ball->GetLastHit() + 1) % 2);

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Silver, TEXT("Out!"));
	}
}

//update points, will cascade into other scoring entities as necessary
void ATennisGameModeBase::UpdatePoints(uint8 side)
{
	uint8 winner, loser;
	if (side == HEADS)
	{
		winner = HEADS;
		loser = TAILS;
	}
	else
	{
		winner = TAILS;
		loser = HEADS;
	}
	
	if (!inTiebreak)
	{
		Scoreboard.points[winner]++;
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString::Printf(TEXT("Point Awarded: %d"), Scoreboard.points[winner]));

		//winning a game = first to score 4 points, win by 2
		if (Scoreboard.points[winner] >= 4 && Scoreboard.points[winner] - Scoreboard.points[loser] >= 2)
		{
			UpdateGames(winner, loser);
		}
	}

	else
	{
		Scoreboard.tiebreak[winner]++;

		//tiebreaks are first to (x) points, win by 2
		if(Scoreboard.tiebreak[winner] >= MaxTiebreakPoints && Scoreboard.tiebreak[winner] - Scoreboard.tiebreak[loser] >= 2)
		{
			UpdateGames(winner, loser);
		}
	}

	//TODO: if match not over
	SetupNewPoint();
}

void ATennisGameModeBase::UpdateGames(uint8 winner, uint8 loser)
{
	Scoreboard.games[winner]++;
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString::Printf(TEXT("Game Awarded")));
	
	/*reset points for next game*/
	if (!inTiebreak)
	{
		Scoreboard.points[winner] = 0; Scoreboard.points[loser] = 0;
	}
	else
	{
		Scoreboard.tiebreak[winner] = 0; Scoreboard.tiebreak[loser] = 0;
	}

	//winner at maximum games per set
	if (Scoreboard.games[winner] == MaxGames)
	{
		//must win set by two games
		if (Scoreboard.games[winner] - Scoreboard.games[loser] >= 2)
		{
			UpdateSets(winner);
		}

		//if both sides at max games, play a tiebreaker
		else if(Scoreboard.games[winner] == Scoreboard.games[loser] && PlayTiebreak)
		{
			inTiebreak = true;
		}
	}

	else if (Scoreboard.games[winner] > MaxGames && inTiebreak)
	{
		inTiebreak = false;
		UpdateSets(winner);
	}

	else
	{
		//error case, hopefully shouldn't happen
		//GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Magenta, FString::Printf(TEXT("Game Count: %d"), Scoreboard.games[winner]));
	}

}

void ATennisGameModeBase::UpdateSets(uint8 winner)
{
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString::Printf(TEXT("Set Awarded")));

	//TODO: record score before resetting
	Scoreboard.sets[winner]++;

	Scoreboard.games[HEADS] = 0;
	Scoreboard.games[TAILS] = 0;
	
	//given x sets in total, a side wins if they get ceil(x/2) sets
	if (Scoreboard.sets[winner] >= FMath::CeilToInt((float)MaxSets / 2))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Green, FString::Printf(TEXT("A Player Has Won!")));
	}
}

void ATennisGameModeBase::SetupNewPoint()
{
	if (!(Players.Num() >= 2))
	{GetPlayers();}

	Players[HEADS]->SetToNewPoint(FVector(-1845, 495, 113), false);
	Players[HEADS]->SetCharState(RALLY);
	Players[HEADS]->GetVelocity() = FVector(0, 0, 0);
	
	Players[TAILS]->SetToNewPoint(FVector(1845, -495, 113), false);
	Players[TAILS]->SetCharState(RALLY);
	Players[TAILS]->GetVelocity() = FVector(0, 0, 0);
	
	Ball->Reset();
	//temp stuff
	Ball->SetActorLocation(FVector(0, 0, 250));
	Ball->GetMovement()->Velocity.X = -1000;
	Ball->GetMovement()->Velocity.Z = 500;
}
