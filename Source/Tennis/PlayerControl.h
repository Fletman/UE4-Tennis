// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControl.generated.h"

/**
 * 
 */
UCLASS()
class TENNIS_API APlayerControl : public APlayerController
{
	GENERATED_BODY()
	
private:
	bool gamePaused = false;
	FVector MovementInput = FVector(0, 0, 0);
	FVector AimInput = FVector(0, 0, 0);
	class ATennisPlayer *PlayerChar;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent();

	void SetPlayerChar();

	/*gamepad input handlers (in XBox terms because I'm a filthy console peasant)*/
	void ButtonA();
	void ButtonB();
	void ButtonX();
	void ButtonY();
	void RTrigOn(); void RTrigOff();
	void LTrigOn(); void LTrigOff();
	void RBumpOn(); void RBumpOff();
	void LBumpOn(); void LBumpOff();	
	void LStickX(float val);
	void LStickY(float val);
	void RStickX(float val);
	void RStickY(float val);
	void DUp();
	void DDown();
	void DLeft();
	void DRight();
	void ButtonStart();
};
