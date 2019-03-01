// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControl.h"
#include "TennisPlayer.h"
#include "Engine.h"



void APlayerControl::BeginPlay()
{
	SetPlayerChar();
}

void APlayerControl::Tick(float DeltaTime)
{
	if (!MovementInput.IsZero())
	{
		
	}
}

void APlayerControl::SetPlayerChar()
{
	PlayerChar = Cast<ATennisPlayer>(GetPawn());
	if (!PlayerChar)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7, FColor::Red, FString::Printf(TEXT("No Player Pawn Found")));
	}
}

//input handler setup
void APlayerControl::SetupInputComponent()
{
	//check(InputComponent); //setup key bindings

	Super::SetupInputComponent();

	InputComponent->BindAxis("L-ThumbX", this, &APlayerControl::LStickX);
	InputComponent->BindAxis("L-ThumbY", this, &APlayerControl::LStickY);
	InputComponent->BindAxis("R-ThumbX", this, &APlayerControl::RStickX);
	InputComponent->BindAxis("R-ThumbY", this, &APlayerControl::RStickY);


	InputComponent->BindAction("A-Button", IE_Pressed, this, &APlayerControl::ButtonA);
	InputComponent->BindAction("B-Button", IE_Pressed, this, &APlayerControl::ButtonB);
	InputComponent->BindAction("X-Button", IE_Pressed, this, &APlayerControl::ButtonX);
	InputComponent->BindAction("Y-Button", IE_Pressed, this, &APlayerControl::ButtonY);


	InputComponent->BindAction("L-Trigger", IE_Pressed, this, &APlayerControl::LTrigOn);
	InputComponent->BindAction("R-Trigger", IE_Pressed, this, &APlayerControl::RTrigOn);
	InputComponent->BindAction("L-Bumper", IE_Pressed, this, &APlayerControl::LBumpOn);
	InputComponent->BindAction("R-Bumper", IE_Pressed, this, &APlayerControl::RBumpOn);

	InputComponent->BindAction("L-Trigger", IE_Released, this, &APlayerControl::LTrigOff);
	InputComponent->BindAction("R-Trigger", IE_Released, this, &APlayerControl::RTrigOff);
	InputComponent->BindAction("L-Bumper", IE_Released, this, &APlayerControl::LBumpOff);
	InputComponent->BindAction("R-Bumper", IE_Released, this, &APlayerControl::RBumpOff);


	InputComponent->BindAction("U-Pad", IE_Pressed, this, &APlayerControl::DUp);
	InputComponent->BindAction("D-Pad", IE_Pressed, this, &APlayerControl::DDown);
	InputComponent->BindAction("L-Pad", IE_Pressed, this, &APlayerControl::DLeft);
	InputComponent->BindAction("R-Pad", IE_Pressed, this, &APlayerControl::DRight);

	InputComponent->BindAction("Start-Button", IE_Pressed, this, &APlayerControl::ButtonStart);
}

//select (menu)
void APlayerControl::ButtonA()
{
	GEngine->AddOnScreenDebugMessage(-1, 7, FColor::Blue, TEXT("A pressed"));
}

//back (menu)
void APlayerControl::ButtonB()
{
	GEngine->AddOnScreenDebugMessage(-1, 7, FColor::Blue, TEXT("B pressed"));
}

//TODO: figure out what to do with this lol
void APlayerControl::ButtonX()
{
	GEngine->AddOnScreenDebugMessage(-1, 7, FColor::Blue, TEXT("X pressed"));
}

//TODO: see : ButtonX()
void APlayerControl::ButtonY()
{
	GEngine->AddOnScreenDebugMessage(-1, 7, FColor::Blue, TEXT("Y pressed"));
}

//topspin takeback
void APlayerControl::RTrigOn()
{
	PlayerChar->SetPrep(/*TOPSPIN_SWING*/);
}

//topspin swing
void APlayerControl::RTrigOff()
{
	PlayerChar->Swing(TOPSPIN_SWING);
	PlayerChar->UnsetPrep();
}

//slice/dropshot takeback
void APlayerControl::LTrigOn()
{
	PlayerChar->SetPrep(/*SLICE_SWING*/);
}

//slice/dropshot swing
void APlayerControl::LTrigOff()
{
	if (PlayerChar->aimY < 0)
	{
		PlayerChar->Swing(DROP_SWING);
	}

	else
	{
		PlayerChar->Swing(SLICE_SWING);
	}

	PlayerChar->UnsetPrep();
}

//flat takeback (same at topspin takeback)
void APlayerControl::RBumpOn()
{
	PlayerChar->SetPrep(/*TOPSPIN_SWING*/);
}

//flat swing
void APlayerControl::RBumpOff()
{
	PlayerChar->Swing(FLAT_SWING);
	PlayerChar->UnsetPrep();
}

//lob takeback
void APlayerControl::LBumpOn()
{
	PlayerChar->SetPrep(/*TOPSPIN_SWING*/);
}

//lob swing
void APlayerControl::LBumpOff()
{
	PlayerChar->Swing(LOB_SWING);
	PlayerChar->UnsetPrep();
}

//X-movement
void APlayerControl::LStickX(float val)
{
	uint8 charState = PlayerChar->GetCharState();

	if (charState == RALLY || charState == RETURN)
	{PlayerChar->sideInput = val;}
}

//Y-movement
void APlayerControl::LStickY(float val)
{
	uint8 charState = PlayerChar->GetCharState();

	if (charState == RALLY || charState == RETURN)
	{PlayerChar->forwardInput = val;}
}

//sideways-movement
void APlayerControl::RStickX(float val)
{
	uint8 charState = PlayerChar->GetCharState();

	//no aiming required during these states
	if (charState == SCENE || charState == RETURN)
	{return;}

	PlayerChar->aimX = val * 680; //aiming bounds for -680 <= X <= 680
}

//forward-aiming
void APlayerControl::RStickY(float val)
{
	uint8 charState = PlayerChar->GetCharState();

	//no aiming required during these states
	if (charState == SCENE || charState == RETURN)
	{return;}

	PlayerChar->aimY = val;
}

//TODO: combine D-Pad with sticks?
//menu navigation
void APlayerControl::DUp()
{

}

//menu navigation
void APlayerControl::DDown()
{

}

//menu navigation
void APlayerControl::DLeft()
{

}

//menu navigation
void APlayerControl::DRight()
{

}

//pause/unpause
void APlayerControl::ButtonStart()
{
	
}


