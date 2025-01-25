#include "DynamicTextureComponent.h"

#include "RHICommandList.h"
#include "Rendering/Texture2DResource.h"

UDynamicTextureComponent::UDynamicTextureComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

bool UDynamicTextureComponent::IsInitialized() {
  return bIsInitialized;
}

void UDynamicTextureComponent::Init(const uint32_t& inWidth, const uint32_t& inHeight) {
  Width = inWidth;
  Height = inHeight;

  Data.SetNumUninitialized(Width * Height * BitsPerPixel);

  Texture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
  Texture->CompressionSettings = TextureCompressionSettings::TC_EditorIcon; // maybe too much memory?
  // Texture->CompressionSettings = TextureCompressionSettings::TC_Default;
  Texture->SRGB = 0;
  Texture->Filter = TextureFilter::TF_Nearest;
  Texture->UpdateResource();

  UpdateTexture();

  bIsInitialized = true;
}

UTexture2D* UDynamicTextureComponent::GetTexture() {
  return Texture;
}

void UDynamicTextureComponent::SetData(TArray<uint8_t>& inData) {
  Data = MoveTemp(inData);
  UpdateTexture();
}

void UDynamicTextureComponent::UpdateTexture() {
  ENQUEUE_RENDER_COMMAND()(
    [this](FRHICommandListImmediate& RHICmdList) {
      if (!this || !Texture) return;

      FUpdateTextureRegion2D region(0, 0, 0, 0, Width, Height);

      RHIUpdateTexture2D(
        Texture->GetResource()->GetTexture2DRHI(),
        0,
        region,
        Width * BitsPerPixel, // pitch
        Data.GetData()
      );
    }
  );
}