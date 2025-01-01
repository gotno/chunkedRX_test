#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Chunked.generated.h"


UCLASS()
class RXCHUNK_API UChunked : public UObject {
	GENERATED_BODY()
	
public:
  virtual void Init(uint32_t inId, size_t TotalSize, int32_t inNumChunks);
  void AddChunk(TArray<uint8_t>& Chunk, int32_t ChunkNum, size_t ChunkSize);

protected:
  uint32_t id;
  TArray<uint8_t> Data;

private:
  int32_t NumChunks;
  TSet<uint32_t> ReceivedChunks;

  bool HasChunk(int32_t ChunkNum);
  void MarkChunkReceived(int32_t ChunkNum);
  bool AllChunksReceived();

  virtual void Finish();
};
