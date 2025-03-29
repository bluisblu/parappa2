#ifndef PRLIB_DATABASE_H
#define PRLIB_DATABASE_H

class PrObjectDatabase {
public:
    void Initialize();
    void Cleanup();
private:
    char unk0[0x10];
};

extern PrObjectDatabase prObjectDatabase;

#endif