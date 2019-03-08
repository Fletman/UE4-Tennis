// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerControl.h"
#include "AIControl.h"
#include "TennisPlayer.h"
#include "TennisBall.h"
#include "GameFramework/GameModeBase.h"
#include "TennisGameModeBase.generated.h"

/*serving side flag*/
#define DEUCE_SIDE 0
#define AD_SIDE 1

/*court side flags (relative to player start in editor)*/
#define NEAR_SIDE 0
#define FAR_SIDE 1

UCLASS()
class TENNIS_API ATennisGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATennisGameModeBase();

	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UWorld *World;
	ATennisBall *Ball;

	ATennisPlayer *Players[2] = {nullptr, nullptr};

	typedef struct ServeInfo
	{
		uint8 currentServer; //which player is serving
		uint8 serveSide; //whether to serve from deuce or ad side
		uint8 courtSide; //whether server is at near or far end of court (relative to player start in editor)
	} ServeInfo;


	/*NOTE: See macros for array indices*/
	//struct containing tennis scoring schema: points, games, sets, and (if necessary) tiebreak points
	typedef struct Score
	{
		bool inTiebreak;
		uint8 setsPlayed;
		ServeInfo serveInfo;
		unsigned short points[2]; //points per game
		unsigned short games[2]; //games per set
		unsigned short sets[2];
		unsigned short tiebreak[2]; //points in a tiebreak (if necessary)

		Score()
		{
			points[HEADS] = 0; points[TAILS] = 0;
			games[HEADS] = 0; games[TAILS] = 0;
			tiebreak[HEADS] = 0; tiebreak[TAILS] = 0;
			sets[HEADS] = 0; sets[TAILS] = 0;

			setsPlayed = 0;
			inTiebreak = false;

			serveInfo.currentServer = HEADS;
			serveInfo.serveSide = DEUCE_SIDE;
			serveInfo.courtSide = NEAR_SIDE;

		};

		Score(uint8 startingServer)
		{
			points[HEADS] = 0; points[TAILS] = 0;
			games[HEADS] = 0; games[TAILS] = 0;
			tiebreak[HEADS] = 0; tiebreak[TAILS] = 0;
			sets[HEADS] = 0; sets[TAILS] = 0;

			setsPlayed = 0;
			inTiebreak = false;

			serveInfo.currentServer = startingServer;
			serveInfo.serveSide = DEUCE_SIDE;

			startingServer == HEADS ?
				serveInfo.courtSide = NEAR_SIDE:
				serveInfo.courtSide = FAR_SIDE;
		};
	} Score;
	Score Scoreboard; //score tracker

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
		bool ShouldPlayTiebreak = true;

	UPROPERTY(EditAnywhere, Category = "Game Ball")
		//tennis ball class to spawn
		TSubclassOf<class ATennisBall> gameBall;

	void ScoreInit(); //intialize scoreboard to all zeroes
	void SpawnBall(); //spawn tennis ball

	void UpdateGames(uint8 winner, uint8 loser);
	void UpdateSets(uint8 winner);
	virtual void SetupNewPoint(); //reset actors to start new point
	virtual void GetPlayers();

public:
	UFUNCTION(BlueprintCallable)
		//on bounce, ball reports its position for in-bounds check
		//NOTE: must be overridden for doubles game-modes, default only checks singles bounds
		virtual void BoundsCheck(FVector BallLocation);

	UFUNCTION(BlueprintCallable)
		//update point, will cascade into other score updates as necessary
		void UpdatePoints(uint8 side);
};


