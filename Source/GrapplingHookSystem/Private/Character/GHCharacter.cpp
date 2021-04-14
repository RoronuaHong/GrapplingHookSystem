// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GHCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Point/GrapplePoint.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "CableComponent.h"

// Sets default values
AGHCharacter::AGHCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	RopeComp = CreateDefaultSubobject<UCableComponent>(TEXT("RopeComp"));
	KunaiComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KunaiComp"));

	RopeComp->SetAttachEndTo(this, FName(KunaiComp->GetName()));

	FRotator Rotation;

	Rotation.Yaw = 0.0f;
	Rotation.Pitch = -90.0f;
	Rotation.Roll = 0.0f;

	KunaiComp->SetWorldRotation(Rotation);

	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);
	RopeComp->SetupAttachment(GetMesh(), TEXT("hand_l"));
	KunaiComp->SetupAttachment(RootComponent);

	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);

	DetectionRadius = 2000.0f;
	GrappleThrowDistance = 1200.0f;

	// Get Grapple Wave.
	static ConstructorHelpers::FObjectFinder<USoundWave> GrappleWaveObj(TEXT("SoundWave'/Game/Sounds/A_Grapple.A_Grapple'"));

	if(GrappleWaveObj.Object) {
		GrappleWave = GrappleWaveObj.Object;
	}

	// Get Sound Wave.
	static ConstructorHelpers::FObjectFinder<USoundWave> JumpWaveObj(TEXT("SoundWave'/Game/Sounds/A_Jump.A_Jump'"));

	if(JumpWaveObj.Object) {
		JumpWave = JumpWaveObj.Object;
	}
}

// Called when the game starts or when spawned
void AGHCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	GHCharacter = Cast<AGHCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	GHCharacter->GetCharacterMovement()->GravityScale = 1.0f;
}

// Called every frame
void AGHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForGrapplePoints();
}

void AGHCharacter::MoveForward(float Value) {
	AddMovementInput(GetActorForwardVector() * Value);
}

void AGHCharacter::MoveRight(float Value) {
	AddMovementInput(GetActorRightVector() * Value);
}

void AGHCharacter::BeginCrouch() {
	Crouch();

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AGHCharacter::EndCrouch() {
	UnCrouch();
}

void AGHCharacter::BeginJump() {
	Jump();
}

// Called to bind functionality to input
void AGHCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGHCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGHCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AGHCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AGHCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AGHCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("UnCrouch", IE_Released, this, &AGHCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGHCharacter::BeginJump);

	PlayerInputComponent->BindAction("Grapple", IE_Pressed, this, &AGHCharacter::StartGrapplingMovement);
}

void AGHCharacter::CheckForGrapplePoints() {
	FCollisionShape MySphere = FCollisionShape::MakeSphere(DetectionRadius);
	FCollisionObjectQueryParams COQP;
	TArray<FHitResult> OutHits;
	FHitResult OutHit;
	TArray<AActor*> IgnoreArray;

	IgnoreArray.Add(GHCharacter);
	IgnoreArray.Add(GetOwner());

	// 使用 SweepMultiByObjectType 进行检测
	COQP.AddObjectTypesToQuery(ECC_GameTraceChannel1);

	const bool bIsFound = GetWorld()->SweepMultiByObjectType(OutHits, GHCharacter->GetActorLocation(), GHCharacter->GetActorLocation(), FQuat::Identity, COQP, MySphere);

	DeactivateGrapplePoint();

	if(bIsFound) {
		HighestDotProduct = 0.7f;

		for(const FHitResult Item : OutHits) {
			if(Item.GetActor()) {
				const FVector ForwardResult = CameraComp->GetForwardVector();
				const FVector CameraResult = (Item.GetActor()->GetActorLocation() - CameraComp->GetComponentLocation()).GetSafeNormal();
				float DotProduct = FVector::DotProduct(ForwardResult, CameraResult);

				if(DotProduct > HighestDotProduct) {
					HighestDotProduct = DotProduct;
					DetectedActor = Item.GetActor();
				}
			}
		}

		if(DetectedActor) {
			DeactivateGrapplePoint();

			GrapplePointRef = Cast<AGrapplePoint>(DetectedActor);

			if(GrapplePointRef) {
				ActivateGrapplePoint();
			}
		}
	}
}

void AGHCharacter::ActivateGrapplePoint() {
	FHitResult OutHit;

	const TArray<AActor*> IgnoreArray;
	FCollisionQueryParams TraceParams;
	FCollisionObjectQueryParams COQP;

	FVector StartVector = CameraComp->GetComponentLocation();
	FVector EndVector = DetectedActor->GetActorLocation();

	COQP.AddObjectTypesToQuery(ECC_Visibility);

	// 实现看得见的点亮功能
	const bool bIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, StartVector, EndVector, ECC_Visibility, TraceParams);
	//DrawDebugLine(GetWorld(), StartVector, EndVector, FColor::Blue, false, 1.0f);

	if(bIsHit) {
		if(DetectedActor == OutHit.GetActor()) {
			GrapplePointRef->Activate(this);
		}
	}
}

void AGHCharacter::DeactivateGrapplePoint() {
	if(GrapplePointRef) {
		GrapplePointRef->Dectivate();
		GrapplePointRef = nullptr;
	}
}

void AGHCharacter::PlayGrappleWave()
{
	UGameplayStatics::PlaySound2D(this, GrappleWave);
}

void AGHCharacter::PlayJumpWave()
{
	UGameplayStatics::PlaySound2D(this, JumpWave);
}

void AGHCharacter::StartGrapplingMovement()
{
	PlayJumpWave();

	//if(GHCharacter) {
	//	GHCharacter->GetCharacterMovement()->GravityScale = 0.0f;
	//	GHCharacter->GetCharacterMovement()->StopMovementImmediately();

	//	StartPos = GHCharacter->GetActorLocation();

	//	GHCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	//	MovingWithGrapple = true;
	//}
}

void AGHCharacter::MoveRope()
{

}

float AGHCharacter::GetDetectionRadius() {
	return DetectionRadius;
}

float AGHCharacter::GetGrappleThrowDistance() {
	return GrappleThrowDistance;
}
