// Fill out your copyright notice in the Description page of Project Settings.

#include "TennisPlayer.h"
#include "TennisRacquet.h"
#include "TennisGameModeBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine.h"


// Sets default values
ATennisPlayer::ATennisPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera")); //actor camera
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); //Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->bUsePawnControlRotation = false; //Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void ATennisPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	BodyMesh = this->GetMesh();
	Racquet = GetWorld()->SpawnActor<ATennisRacquet>(RacquetRef);
	BodyMesh->GetSocketByName(TEXT("RightPalm"))->AttachActor(Racquet, BodyMesh);
	StrikeRange = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(TEXT("Strike Range")));
	SetBall();
	SetCharState(RALLY);
	SetSide();
}

// Called every frame
void ATennisPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (this->cState)
	{
	case SCENE: //currently in some type of scene, nothing to do
		return;
		break;

	case SERVE: //currently in service motion
		//TODO: serve animation handler here
		break;

	case RETURN: //receiving serve
		//TODO: receive animation & movement constraints here
		break;

	case RALLY: //mid-rally, movement as normal
		Move();
		break;

	case GROUNDSTROKE: //play swing animation

		break;

	case VOLLEY: //play volley animation

		break;

	case PREP: //get character into position for swing/volley
		SwingSetUp();
		break;

	default: //error case, (hopefully) shouldn't happen
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Invalid Character State: %d"), this->cState));
		return;
	}

	TargetLock(Ball);
}

//character movement
void ATennisPlayer::Move()
{
	if (forwardInput == 0 && sideInput == 0) //if no movement input, nothing to do
	{return;}

	// find out which way character is facing
	const FRotator Rotation = this->CameraBoom->GetComponentRotation(); //forwards direction based on camera angle
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	 
	//determine 'forward' and 'sideways' relative to character
	const FVector forwardDirec = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector sideDirec = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//move character
	//TODO: movement animations
	AddMovementInput(forwardDirec, forwardInput * PLYR_SPEED);
	AddMovementInput(sideDirec, sideInput * PLYR_SPEED);
}


void ATennisPlayer::Swing()
{
	FVector swingImpulse;
	uint8 spinType;
	//UAnimMontage *animation;

	//set animation instance by swing pattern
	//set ball impulse & spin type by swing pattern
	switch (currentSwingType)
	{
	case FLAT_SWING: //set ball path type to flat, swing animation to flat
		//animation = 
		spinType = FLAT;
		break;

	case TOPSPIN_SWING: //set ball path type to topspin, swing animation to topspin
		//animation = 
		spinType = TOPSPIN;
		break;

	case SLICE_SWING: //set ball path type to sidespin, swing animation to slice
		//animation = 
		spinType = SIDESPIN;
		break;

	case LOB_SWING: //set ball path type to lob, swing animation to lob
		//animation = 
		spinType = TOPSPIN;
		break;

	case DROP_SWING: //set ball path to backspin, swing animation to slice
		//animation = 
		spinType = BACKSPIN;
		break;

	default: //error case, (hopefully) shouldn't happen
			 //TODO: debug statements
		return;
	}

	char sideModifier; //multiplier for opposite of this character's current side (1 or -1)
	this->side == HEADS ? sideModifier = 1 : sideModifier = -1;

	//spawn target for ball to home towards
	FVector tgtPoint = FVector(1100 * sideModifier, aimX, 27.5);
	ABallTarget *homingTgt = GetWorld()->SpawnActor<ABallTarget>(Target, tgtPoint, FRotator(0, 0, 0));

	//Racquet->SetSwing(spinType, CalculateImpulse(homingTgt, type), homingTgt); //set swing properties to be applied /if/ racquet contacts ball
	Ball->SetPath(spinType, CalculateImpulse(homingTgt), homingTgt); //this is temporary, will remove once racquet collisions are properly done
	Ball->SetLastHit(this->GetSide());

	//this->BodyMesh->PlayAnimation(animation, false);
	GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Orange, tgtPoint.ToString());
}

FVector ATennisPlayer::CalculateImpulse(ABallTarget * tgt)
{
	FVector tgtLoc = tgt->GetActorLocation();
	FVector srcLoc = Ball->GetActorLocation();

	float yDiff = FMath::Abs<float>(tgtLoc.Y - srcLoc.Y);
	float xDiff = FMath::Abs<float>(tgtLoc.X - srcLoc.X);
	float hypotenuse = FMath::Sqrt(FMath::Square<float>(xDiff) + FMath::Square<float>(yDiff));
	float angle;
	
	hypotenuse == 0 ? angle = 0 : angle = FMath::Asin(yDiff / hypotenuse);

	float strikeSpeed = 0, zImpulse = 0;

	FVector ballLoc = Ball->GetActorLocation();
	FVector charLoc = this->GetActorLocation();

	//TODO: variable zImpulse
	switch (currentSwingType)
	{
	case FLAT_SWING:
		strikeSpeed = 2000;
		zImpulse = 325;
		break;

	case TOPSPIN_SWING:
		strikeSpeed = 1250;

		if (FMath::Abs<float>(charLoc.X) >= 2000)
		{strikeSpeed += 375;}

		if (ballLoc.Z <= 200)
		{zImpulse = 750;}

		else if (ballLoc.Z <= 275)
		{zImpulse = 625;}

		else
		{zImpulse = 500;}

		break;

	case SLICE_SWING:
		strikeSpeed = 1125;
		zImpulse = 325;
		break;

	case LOB_SWING:
		strikeSpeed = 750;
		zImpulse = 2125;
		break;

	case DROP_SWING:
		strikeSpeed = 375;
		zImpulse = 375;
		break;

	default: //error case, (hopefully shouldn't happen)
		break;
	}

	float xImpulse = strikeSpeed * FMath::Cos(angle);
	if (tgtLoc.X < srcLoc.X)
	{
		xImpulse *= -1;
	}

	float yImpulse = strikeSpeed * FMath::Sin(angle);
	if (tgtLoc.Y < srcLoc.Y)
	{
		yImpulse *= -1;
	}

	Ball->GetMovement()->HomingAccelerationMagnitude = FMath::Max<float>(strikeSpeed, PLYR_SPEED);
	return FVector(xImpulse, yImpulse, zImpulse);
}

void ATennisPlayer::SetSide()
{
	this->GetActorLocation().X < 0 ? side = HEADS : side = TAILS;
}

void ATennisPlayer::SetPrep(int8 newSwingType)
{
	this->currentSwingType = newSwingType;
	prevState = cState;
	cState = PREP;
}

void ATennisPlayer::UnsetPrep()
{
	currentSwingType = -1;
	cState = RALLY; //temporary band-aid to get around other issues, but may keep if no other problems discovered
	//state = prevState;
	//prevState = -1;
}

//train character & camera on game ball
void ATennisPlayer::TargetLock(AActor *lookTgt)
{
	FVector locationDiff = lookTgt->GetActorLocation() - this->Camera->GetComponentLocation();

	float width, height; //width & height hypotenuses
	float newYaw; //rotation about Z-axis
	float newPitch; //rotation about Y-axis

	width = FMath::Sqrt(FMath::Square(locationDiff.X) + FMath::Square(locationDiff.Y));
	height = FMath::Sqrt(FMath::Square(width) + FMath::Square(locationDiff.Z));

	//NOTE: trig functions return radians
	newYaw = FMath::Asin(locationDiff.Y / width);
	newPitch = FMath::Asin(locationDiff.Z / height);

	//radians to degrees
	newYaw *= (180 / PI);
	newPitch *= (180 / PI);

	//angle quadrant appropriation
	if (locationDiff.X < 0)
	{
		newYaw *= -1;
		newYaw += 180;
	}

	this->CameraBoom->SetWorldRotation(FRotator(newPitch, newYaw, 0.0f)); //rotate camera stand about X-axis and Z-axis
	
	//if character isn't moving, rotate body to track ball
	if (this->GetVelocity().IsNearlyZero())
	{
		//for some (unfixable >:/) reason, mesh starting direction is off by 90 deg
		this->BodyMesh->SetWorldRotation(FRotator(0.0f, newYaw - 90, 0.0f), true); //rotated body mesh to face target
	}	
}

//get character into swinging range
void ATennisPlayer::SwingSetUp()
{
	FVector ballSpeed = Ball->GetMovement()->Velocity;

	/*cases of ball not traveling towards from character, nothing to do*/
	bool moveAway1 = (ballSpeed.X < 0 && side == TAILS);
	bool moveAway2 = (ballSpeed.X > 0 && side == HEADS);
	if(moveAway1 || moveAway2 || ballSpeed.X == 0)
	{return;}

	//in-range check
	if (this->StrikeRange->OverlapComponent(Ball->GetActorLocation(), Ball->GetActorQuat(), Ball->GetMesh()->GetCollisionShape()))
	{
		Swing();
		UnsetPrep(); //TODO: find a way to combine this with unsetting via button release, preferably without crashing the game
		return;
	}

	FVector ballLoc = Ball->GetActorLocation();
	FVector charLoc = BodyMesh->GetComponentLocation();

	//expected time until ball is in range of character
	float travelTime = FMath::Abs<float>(charLoc.X - ballLoc.X) / FMath::Abs<float>(ballSpeed.X);
	
	//expected Y-coordinate at landing time
	float expectedY = ballLoc.Y + (ballSpeed.Y * travelTime);

	if (expectedY + 40 < charLoc.Y)
	{
		if (side == TAILS)
		{
			AddMovementInput(FVector(0, 180, 0), -PLYR_SPEED);
		}
		else
		{
			AddMovementInput(FVector(0, -180, 0), PLYR_SPEED);
		}
	}

	else if (expectedY - 40 > charLoc.Y)
	{
		if (side == TAILS)
		{
			AddMovementInput(FVector(0, 180, 0), PLYR_SPEED);
		}
		else
		{
			AddMovementInput(FVector(0, -180, 0), -PLYR_SPEED);
		}
	}

	else
	{
		//character in range for expected ball location, hold position
		sideInput = 0;
		forwardInput = 0;
	}

	//TODO: prep animations here
}

void ATennisPlayer::SetToNewPoint(FVector location, bool isServing)
{
	this->SetActorLocation(location);
	
	//temp change for testing purposes
	/*
	if (isServing)
	{SetCharState(SERVE);}
	
	else
	{SetCharState(RETURN);}
	*/
	
	this->SetCharState(RALLY);
}

//find ball in game world
void ATennisPlayer::SetBall()
{
	TArray<AActor*> FoundBall; //array of ball references (only one needed, will look into more space-efficient method...)

	//find all ball actors
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATennisBall::StaticClass(), FoundBall);
	for (auto Actor : FoundBall)
	{
		Ball = Cast<ATennisBall>(Actor);

		if (Ball) //sanity check
		{break;}

		else
		{Ball = nullptr;}
	}
}

