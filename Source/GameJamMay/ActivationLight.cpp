// Fill out your copyright notice in the Description page of Project Settings.

#include "ActivationLight.h"



// Sets default values for this component's properties
UActivationLight::UActivationLight()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UActivationLight::BeginPlay()
{
	Super::BeginPlay();

	Light = this->GetOwner()->FindComponentByClass<USpotLightComponent>();
}


// Called every frame
void UActivationLight::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetMassofTotalObjects() > MassForTrigger) {
		Light->LightColor = FColor::Green;
		Light->Intensity = 15.f;
		Light->PropagateLightingScenarioChange(); 

	}
	else {
		Light->LightColor = FColor::Red;
		Light->PropagateLightingScenarioChange();
	}
	// ...
}

float UActivationLight::GetMassofTotalObjects(){
	float TotalMass = 0.f;
	
	TArray<AActor*> Overlappingactors;
	Pressure->GetOverlappingActors(OUT Overlappingactors);
	UE_LOG(LogTemp, Warning, TEXT("WTV"));
	for (AActor *actors : Overlappingactors) {
		TotalMass += actors->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("The actor %s has a mass of : %f"), *actors->GetName(), actors->FindComponentByClass<UPrimitiveComponent>()->GetMass());
	}

	return TotalMass;
}

