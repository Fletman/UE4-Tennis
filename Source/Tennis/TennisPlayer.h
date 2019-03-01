// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TennisBall.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "TennisPlayer.generated.h"

/*Player States During Gameplay*/
#define SCENE 0 //cutscene (?)
#define SERVE 1 //player in service motion
#define RETURN 2 //player returning serve
#define RALLY 3 //player currently in rally, normal movement
#define GROUNDSTROKE 4 //groundstroke animation
#define VOLLEY 5 //volley animation
#define PREP 6 //take control of character to begin strike prep, based on previous flag (GROUDSTROKE or VOLLEY)

/*Swing Types, Used for animations*/
#define FLAT_SWING 0
#define TOPSPIN_SWING 1
#define SLICE_SWING 2
#define LOB_SWING 3
#define DROP_SWING 4

//speed of player movement
//TODO: is this even necessary?
#define PLYR_SPEED 1

UCLASS()
class TENNIS_API ATennisPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATennisPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

/*-----------END OF TEMPLATE------------*/

private:
	//pressure during rally affecting movement
	uint32 pressure;
	
	uint8 state = SCENE, prevState = SCENE; //flag denoting state of character
	char side; //flag for character side, used for position calculations (1 or -1)

	//flag indicating character should square up for hit
	bool prep = false;

	//reference to player racquet
	class ATennisRacquet *Racquet = nullptr;

	//calculate directional impulse towards taret
	FVector CalculateImpulse(ABallTarget *tgt, uint8 swingType);

	//get side of court that character is currently on
	void SetSide();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		//camera stand
		class USpringArmComponent *CameraBoom;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
		//actor camera
		class UCameraComponent *Camera;

	UPROPERTY(EditAnywhere, Category = "Targeting")
		//ball target location
		TSubclassOf<class ABallTarget> Target;

	UPROPERTY(EditAnywhere, Category = "Mesh")
		//actor mesh
		class USkeletalMeshComponent *BodyMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh")
		//actor mesh
		TSubclassOf<ATennisRacquet> RacquetRef;

public:
	int speedMult = 2;
	float sideInput = 0, forwardInput = 0; //movement input receivers from a Controller
	float aimX = 0, aimY = 0; //aiming input from a Controller

	//reference to game ball
	class ATennisBall *Ball = nullptr;

	//character movements
	void Move();
	void Swing(uint8 type);

	//setters for having character get into targeting range
	void SetPrep();
	void UnsetPrep();

	void SetBall(); //set reference to game ball
	void TargetLock(AActor *lookTgt); //lock actor camera to game ball
	void SwingSetUp(); //player gets in swinging range of ball

	void SetToNewPoint(FVector location, bool isServing); //set player to new location to start a new point

	UFUNCTION(BlueprintCallable, Category = "Equipment")
		//set reference to actor racquet
		void EquipRacquet(ATennisRacquet *newRac)
		{
			this->Racquet = newRac;
		}

	UFUNCTION(BlueprintCallable, Category = "Equipment")
		//retrieve ball reference
		ATennisRacquet* GetRacquet()
		{
			return this->Racquet;
		}

	UFUNCTION(BlueprintCallable, Category = "Character State")
		//set state of character, determining animations/actions
		void SetCharState(uint8 newState)
		{
			this->state = newState;
		}

	UFUNCTION(BlueprintCallable, Category = "Character State")
		//retrieve current animation/action state of character
		uint8 GetCharState()
		{
			return this->state;
		}
};
