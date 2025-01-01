#include "ChunkedTest.h"

void UChunkedTest::Finish() {
  OnComplete.Broadcast(id, Data);
}