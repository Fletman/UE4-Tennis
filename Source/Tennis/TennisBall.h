// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallTarget.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "TennisBall.generated.h"

/*Spin Types*/
#define FLAT 0
#define TOPSPIN 1
#define BACKSPIN 2
#define SIDESPIN 3

UCLASS()
class TENNIS_API ATennisBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATennisBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

/*------------END OF TEMPLATE--------------*/
private:
	uint8 spinType; //ball spin type
	uint8 bounces; //bounce count
	bool inRally; //whether ball is in rally, affects bounce calculations and all stemming from that
	ABallTarget *homeTgt; //reference to actor used for homing

protected:
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		//static mesh for ball
		class UStaticMeshComponent *BallMesh;

	UPROPERTY(EditAnywhere, Category = "Flight Path", meta = (AllowPrivateAccess = "true"))
		//projectile movement handler
		class UProjectileMovementComponent *Movement;

public:
	UFUNCTION(BlueprintCallable)
		//set ball state
		void SetState(bool newState)
		{
			this->inRally = newState;
		}

	UFUNCTION(BlueprintCallable)
		//get ball state
		bool IsInRally()
		{return this->inRally;}

	UFUNCTION(BlueprintCallable)
		//collision handler
		uint8 Bounce();

	UFUNCTION(BlueprintCallable)
		//set ball's flight path
		void SetPath(uint8 spin, FVector impulse, ABallTarget *tgt);

	UFUNCTION(BlueprintCallable)
		//get ball's flight path
		UProjectileMovementComponent* GetMovement()
		{return this->Movement;}

	UFUNCTION(BlueprintCallable)
		//get ball's spin type
		uint8 GetSpin()
		{return this->spinType;}

	UFUNCTION(BlueprintCallable)
		//reset ball properties
		void Reset();
};
