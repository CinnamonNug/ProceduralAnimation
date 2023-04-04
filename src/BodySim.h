//
// Created by Cengiz Cimen on 4/04/2023.
//

#ifndef PROCEDURALANIMATION_BODYSIM_H
#define PROCEDURALANIMATION_BODYSIM_H

#include <LegKinematics.h>
#include <ParticleSystem.h>
#include <raylib.h>
#include <PVector.h>


class BodySim {
public:
    LegKinematics lkm;
    ParticleSystem psm;

    void Init() {
        PVector hip = lkm.hip;
        PVector l_shoulder = hip + PVector{-100, -200};
        PVector r_shoulder = hip+ PVector{100, -200};
        PVector r_hand = r_shoulder + PVector{0, 200};
        PVector l_hand = l_shoulder + PVector{0, 200};

        psm.AddParticle({hip, hip,4, 0});   // hip particle
        psm.AddParticle({l_shoulder, l_shoulder,4, DEFAULT_MASS});   // l_shoulder
        psm.AddParticle({r_shoulder, r_shoulder,4, DEFAULT_MASS}); // r_shoulder
        psm.AddParticle({r_hand, r_hand,4, DEFAULT_MASS});   // r_hand
        psm.AddParticle({l_hand, l_hand,4, DEFAULT_MASS});   // l_hand


        float arm_length = PVector{l_shoulder - l_hand}.length();
        psm.AddConstraint({0,1, arm_length});   // hip-l_shoulder
        psm.AddConstraint({0,2, arm_length});   // hip-r_shoulder
        psm.AddConstraint({1,2, 200});           // l_shoulder-r_shoulder
        psm.AddConstraint({2,3, 200});           // r_arm-r_shoulder
        psm.AddConstraint({1,4, 200});           // l_arm-l_shoulder
    }


    void Update() {
        lkm.Update();
        PVector &p_hip = psm.particles.at(0).position;
        p_hip = lkm.hip;
        psm.TimeStep();

    }

    void Draw() {
        lkm.Draw();
        psm.Draw();
        lkm.DrawImgui();
    }

};


#endif //PROCEDURALANIMATION_BODYSIM_H
