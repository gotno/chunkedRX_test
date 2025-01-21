#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Chunked.generated.h"


UCLASS()
class RXCHUNK_API UChunked : public UObject {
	GENERATED_BODY()
	
public:
  uint32_t id;
  TArray<uint8_t> Data;

  virtual void Init(uint32_t inId, int64_t TotalSize, int32_t inNumChunks, int32_t inFullChunkSize);
  void AddChunk(TArray<uint8_t>& Chunk, int32_t ChunkNum);

protected:
  virtual void Finish();

private:
  int32_t NumChunks;
  int32_t FullChunkSize;
  TSet<uint32_t> ReceivedChunks;

  bool HasChunk(int32_t ChunkNum);
  void MarkChunkReceived(int32_t ChunkNum);
  bool AllChunksReceived();

  mutable FCriticalSection DataGuard;
};
