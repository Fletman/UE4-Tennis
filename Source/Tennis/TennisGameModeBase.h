// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerControl.h"
#include "AIControl.h"
#include "TennisPlayer.h"
#include "TennisBall.h"
#include "GameFramework/GameModeBase.h"
#include "TennisGameModeBase.generated.h"


UCLASS()
class TENNIS_API ATennisGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATennisGameModeBase();

private:
	

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UWorld *World;
	ATennisBall *Ball;

	TArray<class ATennisPlayer*> Players;

	bool inTiebreak = false;
	uint8 setsPlayed = 0;

	/*NOTE: See macros for array indices*/
	//struct containing tennis scoring schema: points, games, sets, and (if necessary) tiebreak points
	typedef struct score
	{
		unsigned short points[2]; //points per game
		unsigned short games[2]; //games per set
		unsigned short sets[2];
		unsigned short tiebreak[2]; //points in a tiebreak (if necessary)
	} score;
	score Scoreboard; //score tracker

	//stores references of players in world to Players TArray
	//NOTE: default method only supports singles gamemodes, must be overridden for doubles modes
	virtual void GetPlayers();

	UPROPERTY(EditAnywhere, Category = "Match Settings")
		//max number of games in a set (win by two)
		uint8 MaxGames = 6;

	UPROPERTY(EditAnywhere, Category = "Match Settings")
		//max number of sets in a match (best of x)
		uint8 MaxSets = 3;

	UPROPERTY(EditAnywhere, Category = "Match Settings")
		//number of points in a tiebreak (win by two)
		uint8 MaxTiebreakPoints = 7;

	UPROPERTY(EditAnywhere, Category = "Match Settings")
		//whether to play a tiebreak at end of set
		bool PlayTiebreak = true;

	UPROPERTY(EditAnywhere, Category = "Game Ball")
		//tennis ball class to spawn
		TSubclassOf<class ATennisBall> gameBall;

	void ScoreInit(); //intialize scoreboard to all zeroes
	void SpawnBall(); //spawn tennis ball

	void UpdateGames(uint8 winner, uint8 loser);
	void UpdateSets(uint8 winner);
	virtual void SetupNewPoint(); //reset actors to start new point

public:
	UFUNCTION(BlueprintCallable)
		//on bounce, ball reports its position for in-bounds check
		//NOTE: must be overridden for doubles game-modes, default only checks singles bounds
		virtual void BoundsCheck(FVector BallLocation);

	UFUNCTION(BlueprintCallable)
		//update point, will cascade into other score updates as necessary
		void UpdatePoints(uint8 side);
};


