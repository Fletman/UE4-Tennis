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


ATennisGameModeBase::~ATennisGameModeBase()
{
	//delete Ball;
}

void ATennisGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	ScoreInit();
	SpawnBall();
}

void ATennisGameModeBase::Tick(float DeltaTime)
{

}

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

void ATennisGameModeBase::UpdatePoints(uint8 side)
{
	//TODO: update scoring system here
	

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
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString::Printf(TEXT("Point Awarded: %d"), Scoreboard.points[winner]));

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
