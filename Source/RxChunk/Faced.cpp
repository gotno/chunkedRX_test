#include "Faced.h"

#include "DynamicTextureComponent.h"
#include "ChunkedTexture.h"

#include "engine/Texture2D.h"

AFaced::AFaced() {
	PrimaryActorTick.bCanEverTick = false;

  StaticMeshComponent =
    CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
  RootComponent = StaticMeshComponent;

  OverlayTextureComponent =
    CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Overlay Texture"));

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

void AFaced::SetSize(float Width, float Height) {
  StaticMeshComponent->SetWorldScale3D(
    FVector(1.f, Width, Height) * ScalingFactor
  );
}

void AFaced::SetBaseTexture(UTexture2D* Texture) {
  BaseTexture = Texture;
  FaceMaterialInstance->SetTextureParameterValue(FName("texture"), BaseTexture);
}

void AFaced::SetOverlayData(UChunkedTexture* ChunkedTexture) {
  if (!OverlayTextureComponent->IsInitialized()) {
    OverlayTextureComponent->Init(ChunkedTexture->Width, ChunkedTexture->Height);
    FaceMaterialInstance->SetTextureParameterValue(
      FName("overlay"),
      OverlayTextureComponent->GetTexture()
    );
  }

  OverlayTextureComponent->SetData(ChunkedTexture->Data);
}