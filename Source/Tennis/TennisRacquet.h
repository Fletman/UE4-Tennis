// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TennisRacquet.generated.h"

UCLASS()
class TENNIS_API ATennisRacquet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATennisRacquet();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

/*----------END OF TEMPLATE-----------*/

private:
	uint8 spinSetting;
	FVector impulseSetting;
	ABallTarget *Target;
	ATennisPlayer *Owner;

protected:
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		//racquet mesh
		class UStaticMeshComponent *RacquetMesh;

public:
	UFUNCTION(BlueprintCallable, Category = "Swing", meta = (AllowPrivateAccess = "true"))
		void SetSwing(uint8 spin, FVector impulse, ABallTarget *tgt) //set swing info
		{
			this->spinSetting = spin;
			this->impulseSetting = impulse;
			this->Target = tgt;
		}

	/*getters*/
	UFUNCTION(BlueprintCallable, Category = "Swing")
		uint8 GetSpin()
		{return this->spinSetting;}

	UFUNCTION(BlueprintCallable, Category = "Swing")
		FVector GetImpulse()
		{return this->impulseSetting;}

	UFUNCTION(BlueprintCallable, Category = "Swing")
		ABallTarget* GetTarget()
		{return this->Target;}


	UFUNCTION(BlueprintCallable, Category = "Owner")
		void SetRaqOwner(ATennisPlayer *own)
		{this->Owner = own;}

	UFUNCTION(BlueprintCallable, Category = "Owner")
		ATennisPlayer* GetRaqOwner()
		{return this->Owner;}
};
