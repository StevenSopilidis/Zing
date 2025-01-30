@0x850ea8dd93ffb153;

struct Request {
    id @0 :UInt64;
    sourceId @1 :UInt32;
    type @2 :UInt32;
    createdAt @3 :UInt64;

    rawBytes @4 :Data;

    status @5 :RequestProcessingStage;
    processedAt @6 :UInt64;
}

enum RequestProcessingStage {
    pending @0;
    processing @1;
    completed @2;
    error @3;
}