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
	
	SpawnBall();
	ScoreInit();
}

void ATennisGameModeBase::Tick(float DeltaTime)
{

}

//initialize zeroed scoring structs
void ATennisGameModeBase::ScoreInit()
{
	Scoreboard = Score(HEADS);

	//can't have an even number of max sets
	if (MaxSets % 2 == 0)
	{MaxSets++;}
}

void ATennisGameModeBase::SpawnBall()
{
	Ball = World->SpawnActor<ATennisBall>(gameBall, FVector(-1000, 0, 250), FRotator(0, 0, 0));
	Ball->GetMovement()->Velocity.X = -500;
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
	
	if (!Scoreboard.inTiebreak)
	{
		Scoreboard.points[winner]++;
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString::Printf(TEXT("Score: %d - %d"), Scoreboard.points[HEADS], Scoreboard.points[TAILS]));

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

		//alternate server every odd point for tiebreak
		else if ((Scoreboard.tiebreak[HEADS] + Scoreboard.tiebreak[TAILS]) % 2 != 0)
		{
			Scoreboard.serveInfo.currentServer = (Scoreboard.serveInfo.currentServer + 1) % 2;
		}

		//alternate court side every 6 tiebreak points
		else if ((Scoreboard.tiebreak[HEADS] + Scoreboard.tiebreak[TAILS]) % 6 == 0)
		{
			Scoreboard.serveInfo.courtSide = (Scoreboard.serveInfo.courtSide + 1) % 2;
		}
	}

	//given x sets in total, a side wins the match if they get ceil(x/2) sets
	if (Scoreboard.sets[winner] >= FMath::CeilToInt((float)MaxSets / 2))
	{
		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Green, FString::Printf(TEXT("A Player Has Won!")));
		//UGameplayStatics::OpenLevel(World, TEXT("MainMenu"));
	}

	else //begin new point
	{
		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Silver, TEXT("New Point"));

		//even number of points = serve from deuce side, odd = serve from ad side
		if (!Scoreboard.inTiebreak)
		{
			(Scoreboard.points[HEADS] + Scoreboard.points[TAILS]) % 2 == 0 ?
				Scoreboard.serveInfo.serveSide = DEUCE_SIDE :
				Scoreboard.serveInfo.serveSide = AD_SIDE;
		}

		else
		{
			(Scoreboard.tiebreak[HEADS] + Scoreboard.tiebreak[TAILS]) % 2 == 0 ?
				Scoreboard.serveInfo.serveSide = DEUCE_SIDE :
				Scoreboard.serveInfo.serveSide = AD_SIDE;
		}
		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Silver, FString::Printf(TEXT("Switching Sides: %d"), Scoreboard.serveInfo.serveSide));

		SetupNewPoint();
	}
}

void ATennisGameModeBase::UpdateGames(uint8 winner, uint8 loser)
{
	Scoreboard.games[winner]++;
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString::Printf(TEXT("Game Awarded")));
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString::Printf(TEXT("Games: %d - %d"), Scoreboard.games[HEADS], Scoreboard.games[TAILS]));

	//alternate server every game
	Scoreboard.serveInfo.currentServer = (Scoreboard.serveInfo.currentServer + 1) % 2;

	//switch court sides every odd-numbered game
	//TODO: WHY DOES THIS WORK IT SHOULDN'T WORK
	if ((Scoreboard.games[HEADS] + Scoreboard.games[TAILS]) % 2 == 0)
	{
		Scoreboard.serveInfo.courtSide = (Scoreboard.serveInfo.courtSide + 1) % 2;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, TEXT("Switching Ends"));
	}
	
	/*reset points for next game*/
	if (!Scoreboard.inTiebreak)
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
		else if(Scoreboard.games[winner] == Scoreboard.games[loser] && ShouldPlayTiebreak)
		{
			Scoreboard.inTiebreak = true;
		}
	}

	//tiebreak over, set won
	else if (Scoreboard.games[winner] > MaxGames && Scoreboard.inTiebreak)
	{
		Scoreboard.inTiebreak = false;
		UpdateSets(winner);
	}	
}

void ATennisGameModeBase::UpdateSets(uint8 winner)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, FString::Printf(TEXT("Set Awarded")));

	//TODO: record score to billboard before resetting
	Scoreboard.sets[winner]++;

	Scoreboard.games[HEADS] = 0;
	Scoreboard.games[TAILS] = 0;

	//switch court sides at end of set
	Scoreboard.serveInfo.courtSide = (Scoreboard.serveInfo.courtSide + 1) % 2;
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, TEXT("Switching Ends"));
}

void ATennisGameModeBase::SetupNewPoint()
{
	uint8 server = Scoreboard.serveInfo.currentServer;
	uint8 receiver = (Scoreboard.serveInfo.currentServer + 1) % 2;

	//location variables
	float sX, rX, sY, rY;

	/*location modifiers*/
	switch (Scoreboard.serveInfo.courtSide)
	{
		case NEAR_SIDE:
			sX = -1845;
			sY = 1;
			rX = 1845;
			rY = -1;
			break;

		case FAR_SIDE:
			sX = 1845;
			sY = -1;
			rX = -1845;
			rY = 1;
			break;

		default: //error case, (hopefully) shouldn't happen
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Invalid court side: %d"), Scoreboard.serveInfo.courtSide));
			break;
	}
	switch (Scoreboard.serveInfo.serveSide)
	{
		case DEUCE_SIDE:
			sY *= 325;
			rY *= 495;
			break;

		case AD_SIDE:
			sY *= -325;
			rY *= -495;
			break;

		default: //error case, blah blah you know the drill
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Invalid serve side: %d"), Scoreboard.serveInfo.serveSide));
			break;
	}
	
	if (Players[0] == nullptr)
	{GetPlayers();}

	Players[server]->SetToNewPoint(FVector(sX, sY, 115), true);
	Players[receiver]->SetToNewPoint(FVector(rX, rY, 115), false);
	
	Ball->Reset();
	//temp stuff for testing
	Ball->SetActorLocation(FVector(Players[server]->GetActorLocation().X, Players[server]->GetActorLocation().Y, 310));
	float xVel = Players[receiver]->GetActorLocation().X - 200;
	Ball->GetMovement()->Velocity.Set(xVel, 0, 500);
}

void ATennisGameModeBase::GetPlayers()
{
	TArray<AActor*> FoundPlayer;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATennisPlayer::StaticClass(), FoundPlayer);
	for (auto Actor : FoundPlayer)
	{
		ATennisPlayer *p = Cast<ATennisPlayer>(Actor);

		if (p) //sanity check
		{
			Players[p->GetSide()] = p;
		}

		else
		{
			exit(EXIT_FAILURE); //extreme, but this shouldn't happen
		}
	}
}