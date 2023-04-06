//
// Created by Cengiz Cimen on 4/04/2023.
//

#ifndef PROCEDURALANIMATION_BODYSIM_H
#define PROCEDURALANIMATION_BODYSIM_H

#include <LegKinematics.h>
#include <ParticleSystem.h>
#include <raylib.h>
#include <PVector.h>


class BodySim : public ParticleSystem {
public:
    LegKinematics lkm;
    PVector CM_Offset;
    double M;

    BodySim() : CM_Offset(0,0), M(0) {
        PVector hip = lkm.hip;
        PVector l_shoulder = hip + PVector{-100, -200};
        PVector r_shoulder = hip+ PVector{100, -200};
        PVector r_hand = r_shoulder + PVector{200, 0};
        PVector l_hand = l_shoulder + PVector{-200, 0};

        this->AddParticle({hip, hip,4, DEFAULT_MASS});   // hip particle
        this->AddParticle({l_shoulder, l_shoulder,4, DEFAULT_MASS});   // l_shoulder
        this->AddParticle({r_shoulder, r_shoulder,4, DEFAULT_MASS}); // r_shoulder
        this->AddParticle({r_hand, r_hand,4, 1/200.f});   // r_hand
        this->AddParticle({l_hand, l_hand,4, 1/200.f});   // l_hand


        float arm_length = PVector{l_shoulder - l_hand}.length();
        this->AddConstraint({0,1, arm_length});    // hip-l_shoulder
        this->AddConstraint({0,2, arm_length});    // hip-r_shoulder
        this->AddConstraint({1,2, 200});           // l_shoulder-r_shoulder
        this->AddConstraint({2,3, 200});           // r_arm-r_shoulder
        this->AddConstraint({1,4, 200});           // l_arm-l_shoulder

        // Calculate where the centre of mass is relative to the hip
        for (int i = 1; i < 3; i++) {
            double m = 1 / this->particles[i].inv_mass; // inverse mass
            M += m;
            CM_Offset += this->particles[i].position * m;
        }
        CM_Offset /= M;
        CM_Offset = CM_Offset - hip;
    }

    void AccumulateForces() override {
        lkm.Update();

        PVector CM = CM_Offset + lkm.hip;

        PVector torso{0,0};
        for (int i = 1; i < 3; i++) {
            double m = 1 / this->particles[i].inv_mass; // inverse mass
            torso += this->particles[i].position * m;
        }
        torso /= M;

        PVector comToTorso = CM - torso;

        PVector torque = PVector(-comToTorso.y, comToTorso.x); // Torque vector

        this->particles.at(1).acceleration = torque * -1 / this->particles.at(1).inv_mass ;
        this->particles.at(2).acceleration = torque * 1 / this->particles.at(2).inv_mass;


        DrawCircleP(CM, 4, MAGENTA);
        DrawCircleP(torso, 4, MAGENTA);
        PVector torso_tor =  torso + torque;
        DrawLineP(torso, torso_tor, 4.f, MAGENTA);

    }

    void SatisfyConstraints() override {
        for(int j=0; j < 5; j++) {
            for (auto &c : constraints) {
                // Then satisfy (C2)
                PVector& x1 = particles[c.particle_a].position;
                PVector& x2 = particles[c.particle_b].position;
                PVector delta = x2 - x1;
                float deltalength = sqrt(delta.dot(delta));
                float diff = (deltalength-c.length)
                             /(deltalength*(particles[c.particle_a].inv_mass+particles[c.particle_b].inv_mass));
                x1 += delta*diff*particles[c.particle_a].inv_mass;
                x2 -= delta*diff*particles[c.particle_b].inv_mass;
            }

            // hips must be attached
            this->particles.at(0).position = lkm.hip;

            // the hand and body must not cross each other
            // points 1 and 4 left shoulder + hand
            for (int i = 3; i < 5; i++) {
                PVector arm = particles[i].position - particles[0].position;
                if (arm.y <= 0) {
                    // arm is crossing over body, adjust position to satisfy constraint
                    particles[i].position.y = particles[0].position.y;
                }
            }

            // shoulders should never be lower than hips
            for (int i = 1; i < 3; i++) {
                PVector arm = particles[i].position - particles[0].position;
                if (particles[i].position.y >= particles[0].position.y) {
                    // arm is crossing over body, adjust position to satisfy constraint
                    particles[i].position.y = particles[0].position.y;
                }
            }
        }
    }


    void Draw() override {
        lkm.Draw();
        this->DrawSkeleton();
        lkm.DrawImgui();
    }

};


#endif //PROCEDURALANIMATION_BODYSIM_H
