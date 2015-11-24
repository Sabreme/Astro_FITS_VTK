#ifndef HANDMODEL_H
#define HANDMODEL_H


typedef double fingerJoints[5][3];          /// Each finger has 5 joints and 3 pt Vector
typedef double fingerBones[4][4];        /// Each Finger has 4 Bones and  4 pt Vector (Which includes  1 Length)

class HandModeller
{

public:

    void jointRightStartPos(fingerJoints& joints, int finger);
    void jointLeftStartPos(fingerJoints& joints, int finger);
    void boneRightStartPos(fingerBones &bones, int finger);
    void boneLeftStartPos(fingerBones& bones, int finger);


};



#endif // HANDMODEL_H

