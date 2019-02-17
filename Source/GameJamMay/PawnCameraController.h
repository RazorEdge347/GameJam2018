// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Public/GameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "Public/DrawDebugHelpers.h"
#include "PawnCameraController.generated.h"



UCLASS()
class GAMEJAMMAY_API APawnCameraController : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawnCameraController();
	APawnCameraController(FName TagName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float MYaxis;
	float MXaxis;
	float ZoomFactor;

	bool bAreHandsFree;
	bool BZoom;
	bool IsCameraVisible;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void ChangeObjects();

	float axisGrab = 0.f;

	float dist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName NameOfTag;

	FVector GrabAnchorPoint;

	AActor *CurrentGrab;

	FRotator InitialRotation;

	FRotator NewRotation;

	APlayerController *Controller= nullptr;

	UCameraComponent *Camera = nullptr;
	
	FTwoVectors GetTraceEnd();

	const FHitResult GetFirstHitReach();

	void LookHor(float AxisValue);

	void LookVer(float AxisValue);

	void Posses_Pawn();

	void GrabThing(float Val);

	void ReleaseGrab();

	void Zoom();

	void ZoomOut();

};
