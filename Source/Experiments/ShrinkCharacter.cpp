// Fill out your copyright notice in the Description page of Project Settings.


#include "ShrinkCharacter.h"
#include "Engine.h"
// Sets default values
AShrinkCharacter::AShrinkCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a first person camera component.
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// Attach the camera component to our capsule component.
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());
	// Position the camera slightly above the eyes.
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Allow the pawn to control camera rotation.
	FPSCameraComponent->bUsePawnControlRotation = true;
	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);
	// Attach the FPS mesh to the FPS camera.
	FPSMesh->SetupAttachment(FPSCameraComponent);
	// Disable some environmental shadowing to preserve the illusion of having a single mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	CapCom = GetCapsuleComponent();

	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);

}

// Called when the game starts or when spawned
void AShrinkCharacter::BeginPlay()
{
	Super::BeginPlay();


	if (GEngine)
	{
		// Put up a debug message for five seconds. The -1 "Key" value (first argument) indicates that we will never need to update or refresh this message.
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
	}
}
	


// Called every frame
void AShrinkCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	{
		float CurrentScale = CapCom->GetComponentScale().X;
		if (bGrowing)
		{
			// Grow to double size over the course of one second
			CurrentScale -= DeltaTime;
			CurrentScale = FMath::Clamp(CurrentScale, 0.125f, 0.5f);
		}
		else {
			CurrentScale += DeltaTime;
			CurrentScale = FMath::Clamp(CurrentScale, 0.5f, 1.0f);
		}


		// Make sure we never drop below our starting size, or increase past double size.

		CapCom->SetWorldScale3D(FVector(CurrentScale));
	}

	// Handle movement based on our "MoveX" and "MoveY" axes
	{
		if (!CurrentVelocity.IsZero())
		{
			FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
			SetActorLocation(NewLocation);
		}
	}
}

// Called to bind functionality to input
void AShrinkCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AShrinkCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShrinkCharacter::MoveRight);

	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &AShrinkCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AShrinkCharacter::AddControllerPitchInput);

	// Set up "action" bindings.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShrinkCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AShrinkCharacter::StopJump);

	PlayerInputComponent->BindAction("Grow", IE_Pressed, this, &AShrinkCharacter::StartGrowing);
}

	void AShrinkCharacter::MoveForward(float Value)
	{
		// Find out which way is "forward" and record that the player wants to move that way.
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

	void AShrinkCharacter::MoveRight(float Value)
	{
		// Find out which way is "right" and record that the player wants to move that way.
		FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}

	void AShrinkCharacter::StartJump()
	{
		bPressedJump = true;
	}

	void AShrinkCharacter::StopJump()
	{
		bPressedJump = false;
	}

	void AShrinkCharacter::StartGrowing()
	{
		if (!bGrowing) {
			bGrowing = true;
		}
		else {
			bGrowing = false;
		}
	}


