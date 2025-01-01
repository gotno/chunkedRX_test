#include "Faced.h"

#include "engine/Texture2D.h"

AFaced::AFaced() {
	PrimaryActorTick.bCanEverTick = false;

  StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
  RootComponent = StaticMeshComponent;

  static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshBody(MeshRef);
  if (MeshBody.Object) StaticMeshComponent->SetStaticMesh(MeshBody.Object);

  static ConstructorHelpers::FObjectFinder<UMaterial> BaseMaterial(BaseMaterialRef);
  if (BaseMaterial.Object) {
    BaseMaterialInterface = Cast<UMaterial>(BaseMaterial.Object);
  }

  static ConstructorHelpers::FObjectFinder<UMaterial> FaceMaterial(FaceMaterialRef);
  if (FaceMaterial.Object) {
    FaceMaterialInterface = Cast<UMaterial>(FaceMaterial.Object);
  }
}

void AFaced::BeginPlay() {
	Super::BeginPlay();

  if (BaseMaterialInterface) {
    BaseMaterialInstance =
      UMaterialInstanceDynamic::Create(BaseMaterialInterface, this);
    StaticMeshComponent->SetMaterial(0, BaseMaterialInstance);
    BaseMaterialInstance->SetVectorParameterValue(
      FName("color"),
      FColor::Black
    );
  }

  if (FaceMaterialInterface) {
    FaceMaterialInstance =
      UMaterialInstanceDynamic::Create(FaceMaterialInterface, this);
    StaticMeshComponent->SetMaterial(1, FaceMaterialInstance);
    FaceMaterialInstance->SetVectorParameterValue(
      FName("background_color"),
      FColor::Black
    );
  }

}

void AFaced::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AFaced::SetSize(float Width, float Height) {
  StaticMeshComponent->SetWorldScale3D(
    FVector(1.f, Width, Height) * ScalingFactor
  );
}

void AFaced::SetTexture(UTexture2D* Texture) {
  FaceMaterialInstance->SetTextureParameterValue(FName("texture"), Texture);
}