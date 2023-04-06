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
    PVector CM_Offset = PVector{0, 0};
    double M = 0;

    void Init() {
        PVector hip = lkm.hip;
        PVector l_shoulder = hip + PVector{-100, -200};
        PVector r_shoulder = hip+ PVector{100, -200};
        PVector r_hand = r_shoulder + PVector{0, 200};
        PVector l_hand = l_shoulder + PVector{0, 200};

        psm.AddParticle({hip, hip,4, DEFAULT_MASS});   // hip particle
        psm.AddParticle({l_shoulder, l_shoulder,4, DEFAULT_MASS});   // l_shoulder
        psm.AddParticle({r_shoulder, r_shoulder,4, DEFAULT_MASS}); // r_shoulder
        psm.AddParticle({r_hand, r_hand,4, 1/200.f});   // r_hand
        psm.AddParticle({l_hand, l_hand,4, 1/200.f});   // l_hand


        float arm_length = PVector{l_shoulder - l_hand}.length();
        psm.AddConstraint({0,1, arm_length});   // hip-l_shoulder
        psm.AddConstraint({0,2, arm_length});   // hip-r_shoulder
        psm.AddConstraint({1,2, 200});           // l_shoulder-r_shoulder
        psm.AddConstraint({2,3, 200});           // r_arm-r_shoulder
        psm.AddConstraint({1,4, 200});           // l_arm-l_shoulder


        // hip and torso should be the same intially, I know where it is at first so we don't need to calculate;

        for (int i = 0; i < 5; i++) {
            double m = 1 / psm.particles[i].inv_mass; // inverse mass
            M += m;
            CM_Offset += psm.particles[i].position * m;
        }
        CM_Offset /= M;
        CM_Offset = CM_Offset - hip;


    }


    void Update() {
        lkm.Update();
        PVector &p_hip = psm.particles.at(0).position;
        p_hip = lkm.hip;
        PVector CM = CM_Offset + p_hip;

        PVector torso{0,0};
        for (int i = 0; i < 5; i++) {
            double m = 1 / psm.particles[i].inv_mass; // inverse mass
            torso += psm.particles[i].position * m;
        }
        torso /= M;

        PVector comToTorso = torso - CM; // Vector from the center of mass to the torso

//        double theta = std::acos(comToTorso.normalized().dot(PVector(0, -1))); // Angle between the vector and vertical axis
//        double torqueMagnitude = 0.1 * theta; // Torque magnitude proportional to the angle
//        PVector torque = PVector(comToTorso.y, comToTorso.x) * torqueMagnitude; // Torque vector
//
//
//        psm.particles.at(1).acceleration = torque * 1 / psm.particles[1].inv_mass;
//        psm.particles.at(2).acceleration = torque * 1 / psm.particles[2].inv_mass;
//        psm.particles.at(3).acceleration = torque * 1 / psm.particles[3].inv_mass;
//        psm.particles.at(4).acceleration = torque * 1 / psm.particles[4].inv_mass;

        PVector net_torque{0, 0}; // initialize to zero
        for (int i = 0; i < 5; i++) {
            ParticleSystem::Particle &p = psm.particles[i];
            PVector force = p.acceleration * p.inv_mass; // compute force for current particle
            PVector radius = p.position - CM; // compute radius vector for current particle
            float torque = radius.x * force.y - radius.y * force.x; // compute torque scalar for current particle using inline formula
            net_torque += PVector(-torque, torque); // accumulate torque as a 2D vector
        }

        double I = 0.0; // moment of inertia
        for (int i = 0; i < 5; i++) {
            ParticleSystem::Particle &p = psm.particles[i];
            double m = 1.0 / p.inv_mass;
            double r = (p.position - CM).length();
            I += m * r * r;
        }

        double angular_acceleration = net_torque.length() / I;

        double theta = std::acos(comToTorso.normalized().dot(PVector(0, -1))); // Angle between the vector and vertical axis
        double torqueMagnitude = 0.1 * theta; // Torque magnitude proportional to the angle
        PVector torque = PVector(comToTorso.y, comToTorso.x) * torqueMagnitude; // Torque vector

        psm.particles.at(1).acceleration = torque * angular_acceleration * 1 / psm.particles[1].inv_mass;
        psm.particles.at(2).acceleration = torque * angular_acceleration * 1 / psm.particles[2].inv_mass;
        psm.particles.at(3).acceleration = torque *  angular_acceleration * 1 / psm.particles[3].inv_mass;
        psm.particles.at(4).acceleration = torque *angular_acceleration * 1 / psm.particles[4].inv_mass;




        DrawCircleP(CM, 4, MAGENTA);
        DrawCircleP(torso, 4, MAGENTA);
        PVector torso_tor =  torso + torque * 10.f;
        DrawLineP(torso, torso_tor, 4.f, MAGENTA);

        psm.TimeStep();

    }

    void Draw() {
        lkm.Draw();
        psm.Draw();
        lkm.DrawImgui();
    }

};


#endif //PROCEDURALANIMATION_BODYSIM_H
