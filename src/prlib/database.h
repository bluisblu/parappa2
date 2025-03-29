#ifndef PRLIB_DATABASE_H
#define PRLIB_DATABASE_H

#include "prlib/scene.h"

#include <eetypes.h>
#include <libgraph.h>

class PrObjectDatabase {
public:
    void Initialize();
    void Cleanup();
    PrSceneObject* CreateScene(sceGsDrawEnv1 *drawEnv, const char *name, u_int fbp);
    void DeleteScene(PrSceneObject *scene);
public:
    PrSceneObject *unk0;
    char unk4[0xb];
};

extern PrObjectDatabase prObjectDatabase;

#endif