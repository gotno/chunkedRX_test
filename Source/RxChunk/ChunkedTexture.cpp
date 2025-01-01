#include "ChunkedTexture.h"

#include "Engine/Texture2D.h"

void UChunkedTexture::SetDimensions( int32_t inWidth, int32_t inHeight) {
  Width = inWidth;
  Height = inHeight;
}

UTexture2D* UChunkedTexture::MakeTexture() {
  UTexture2D* TextureTarget =
    UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

  uint8* MipData =
    (uint8*)TextureTarget->
      GetPlatformData()->
      Mips[0].BulkData.Lock(LOCK_READ_WRITE);

  FMemory::Memcpy(MipData, Data.GetData(), Width * Height * 4);

  TextureTarget->GetPlatformData()->Mips[0].BulkData.Unlock();
  TextureTarget->UpdateResource();

  return TextureTarget;
}

void UChunkedTexture::Finish() {
  OnComplete.Broadcast(id, MakeTexture());
}