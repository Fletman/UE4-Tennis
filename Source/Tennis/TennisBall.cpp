// Fill out your copyright notice in the Description page of Project Settings.

#include "TennisBall.h"

// Sets default values
ATennisBall::ATennisBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bRunOnAnyThread = true;

	//initialize components
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	SetRootComponent(BallMesh);
}

// Called when the game starts or when spawned
void ATennisBall::BeginPlay()
{
	Super::BeginPlay();	
	lastToHit = 0;
}

// Called every frame
void ATennisBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//called when ball collides with court (see: TennisBallBP)
uint8 ATennisBall::Bounce()
{
	Movement->bIsHomingProjectile = false;
	Movement->Bounciness = 0.75f;
	
	if (Movement->HomingTargetComponent != nullptr)
	{
		ABallTarget *temp = Cast<ABallTarget>(Movement->HomingTargetComponent->GetOwner());
		Movement->HomingTargetComponent->UnregisterComponent();
		Movement->HomingTargetComponent = nullptr;
		temp->Destroy();
	}

	//TODO: find better way to detect net collision; this isn't reliable
	if (this->GetActorLocation().Z > 40) //collision at height above 30 means no contact with ground court, must've hit net
	{
		
		return this->bounces;
	}
	
	return ++this->bounces;
}

//set ball flight path
void ATennisBall::SetPath(uint8 spin, FVector impulse, ABallTarget *tgt)
{
	//case that prev target not already freed (ex. on volleys)
	if(Movement->HomingTargetComponent != nullptr)
	{
		ABallTarget *temp = Cast<ABallTarget>(Movement->HomingTargetComponent->GetOwner());
		Movement->HomingTargetComponent->UnregisterComponent();
		Movement->HomingTargetComponent = nullptr;
		temp->Destroy();
	}

	//set movement properties depending on spin
	switch (spin)
	{
		case FLAT:
			Movement->Bounciness = 0.75f;
			Movement->ProjectileGravityScale = 1;
			break;

		case TOPSPIN:
			Movement->Bounciness = 0.95f;
			Movement->ProjectileGravityScale = 1.25f;
			break;

		case BACKSPIN:
			Movement->Bounciness = 0.3f;
			Movement->ProjectileGravityScale = 0.5f;

			break;

		case SIDESPIN:
			Movement->Bounciness = 0.625f;
			Movement->ProjectileGravityScale = 0.65f;
			break;
			
		default: //error case
			//TODO: error message
			return;
	}
	
	if (tgt != nullptr)
	{
		Movement->bIsHomingProjectile = true; //ball will home towards target
		Movement->HomingTargetComponent = tgt->TargetArea;
	}

	Movement->SetVelocityInLocalSpace(impulse); //apply racquet impulse to set ball speed
	this->bounces = 0; //reset bounce count
}

//reset ball properties
void ATennisBall::Reset()
{
	this->bounces = 0;
	this->Movement->Bounciness = 0.75f;
	this->Movement->ProjectileGravityScale = 1.0f;
	this->spinType = TOPSPIN;
	this->Movement->Velocity = FVector(0, 0, 0);
}