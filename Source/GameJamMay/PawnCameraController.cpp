// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnCameraController.h"


// Sets default values
APawnCameraController::APawnCameraController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

APawnCameraController::APawnCameraController(FName TagName)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APawnCameraController::BeginPlay()
{
	Super::BeginPlay();
	Camera = FindComponentByClass<UCameraComponent>();
	Controller = GetWorld()->GetFirstPlayerController();
	InitialRotation = Camera->GetComponentRotation();
	bAreHandsFree = true;
	
	
	if (Controller == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Dude The pointer for the Controller is missing"));
		return;
	}

	if (Camera == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Dude The pointer for the camera is missing"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("The initial of Rotation of the %s is : %s"), *GetWorld()->GetFirstPlayerController()->GetName(), *InitialRotation.ToString());
	
	

}

// Called every frame
void APawnCameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/// Rotation for the Camera (Still needs fixing)
	FTransform NewTransform = Camera->GetRelativeTransform();
	NewRotation = NewTransform.Rotator();
		
	NewRotation.Yaw = FMath::Clamp(NewRotation.Yaw+MXaxis, 10.f, 170.f);
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + MYaxis, InitialRotation.Pitch-40.f, InitialRotation.Pitch + 80.f);
	
	Camera->SetRelativeRotation(NewRotation);

	if (BZoom)
	{
		
		ZoomFactor += DeltaTime / 0.5f;         //Zoom in over half a second
	}
	else
	{
		ZoomFactor -= DeltaTime / 0.25f;        //Zoom out over a quarter of a second
	}
	ZoomFactor = FMath::Clamp<float>(ZoomFactor, 0.0f, 1.0f);
	//Blend our camera's FOV and our SpringArm's length based on ZoomFactor
	Camera->FieldOfView = FMath::Lerp<float>(90.0f, 60.0f, ZoomFactor);


	DrawDebugLine(
		GetWorld(),
		GetTraceEnd().v1,
		GetTraceEnd().v2,
		FColor(255, 0, 0),
		false, -1, 0,
		12.333
	);

}

// Called to bind functionality to input
void APawnCameraController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputComponent) {
		PlayerInputComponent->BindAxis("Turn", this, &APawnCameraController::LookHor);
		PlayerInputComponent->BindAxis("LookUp", this, &APawnCameraController::LookVer);
		PlayerInputComponent->BindAxis("Grab", this, &APawnCameraController::GrabThing);
		PlayerInputComponent->BindAction("Zoom", EInputEvent::IE_Pressed, this, &APawnCameraController::Zoom);
		PlayerInputComponent->BindAction("Zoom", EInputEvent::IE_Released, this, &APawnCameraController::ZoomOut);
		PlayerInputComponent->BindAction("Passing_Control", EInputEvent::IE_Pressed, this, &APawnCameraController::Posses_Pawn);
		
		

	}

}


FTwoVectors APawnCameraController::GetTraceEnd()
{
	FVector PlayerLocation;
	FRotator PlayerRotation;

	// Gets the player ViewPoint (Location, Rotation)
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerLocation, OUT PlayerRotation);

	//UE_LOG(LogTemp, Warning, TEXT("Location : %s , Roatation : %s"), *PlayerLocation.ToString(), *PlayerRotation.ToString());

	FVector TraceEnd = PlayerLocation + PlayerRotation.Vector()*10000.f;
	return FTwoVectors(PlayerLocation, TraceEnd);
}

const FHitResult APawnCameraController::GetFirstHitReach()
{
	FHitResult Linetracehit;

	GetWorld()->LineTraceSingleByObjectType(OUT Linetracehit, GetTraceEnd().v1, GetTraceEnd().v2, FCollisionObjectQueryParams::AllObjects, FCollisionQueryParams(FName(TEXT("")), false, GetOwner()));

	AActor *FoundActor = Linetracehit.GetActor();

	if (FoundActor != NULL) {
		UE_LOG(LogTemp, Warning, TEXT("You're tracing this object : %s"), *FoundActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Nothing found"));
	}

	return Linetracehit;
}

void APawnCameraController::LookHor(float AxisValue) {
	
	MXaxis = AxisValue;

}

void APawnCameraController::LookVer(float AxisValue) {

	MYaxis = AxisValue;

}

void APawnCameraController::Zoom() {

	BZoom = true;

}

void APawnCameraController::ZoomOut() {
	
	BZoom = false;

}

void APawnCameraController::Posses_Pawn() {

	auto HitResult = GetFirstHitReach();
	APawn *PawnHit = Cast<APawn>(HitResult.GetActor());
	Controller->UnPossess();
	Controller->Possess(PawnHit);
	ChangeObjects();

}

void APawnCameraController::ChangeObjects() {
	
	TArray<AActor*> FoundActors;
	APawnCameraController *dude = nullptr;
	dude = Cast<APawnCameraController>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	UE_LOG(LogTemp, Warning, TEXT("The Pawn changed to: %s"), *dude->NameOfTag.ToString());
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), dude->NameOfTag, FoundActors);
	

	for (auto &actors : FoundActors) {
		actors->GetRootComponent()->ToggleVisibility();
		UE_LOG(LogTemp, Warning, TEXT("The Actor who Changed is : %s"), *actors->GetName());
	}

	

}

void APawnCameraController::GrabThing(float Val)
{

	if (Val >= 0.9f)
	{
		FHitResult H;
		if (bAreHandsFree) {
			H = GetFirstHitReach();
			dist = H.Distance;
			CurrentGrab = H.GetActor();
			UE_LOG(LogTemp, Warning, TEXT("CAST"));
		}

		if (CurrentGrab && CurrentGrab != nullptr) {
			if (CurrentGrab->ActorHasTag("PickUpAble")) {
				UE_LOG(LogTemp, Warning, TEXT("GRAB!"));
				bAreHandsFree = false;

				GrabAnchorPoint = CurrentGrab->GetActorLocation();
				
				FVector newMove = GetActorLocation() + Camera->GetComponentRotation().Vector() * dist; /*FMath::Lerp(CurrentGrab->GetActorLocation(), GetActorRotation().Vector()*100.f, GetWorld()->DeltaTimeSeconds * 10);*/

				CurrentGrab->SetActorLocation(newMove);
			}
		}
	}
	else {
		bAreHandsFree = true;
		CurrentGrab = nullptr;
	}
}




