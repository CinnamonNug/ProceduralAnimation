//
// Created by Cengiz Cimen on 1/04/2023.
//

#ifndef PROCEDURALANIMATION_PARTICLESYSTEM_H
#define PROCEDURALANIMATION_PARTICLESYSTEM_H

#include "PVector.h"
#include <raylib.h>
#include <vector>


#define DEFAULT_ACCELERATION PVector{0, 980.f}
#define DEFAULT_MASS (1/100.f)

class ParticleSystem {
public:
    // Variables
    struct Particle;
    struct Constraint;
    std::vector<Particle> particles;
    std::vector<Constraint> constraints;
    float fTimeStep = 1/60.f;

    virtual ~ParticleSystem() = default;

    void TimeStep() {
        AccumulateForces();
        Verlet();
        SatisfyConstraints();
    }

    void AccumulateForces() {
        for (auto &p : particles) {
            p.acceleration = DEFAULT_ACCELERATION;
        }

        Particle &p2 = particles[particles.size() - 1];
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            PVector temp = PVector{GetMousePosition().x, GetMousePosition().y} - p2.position;
            p2.acceleration = temp * 100;
        } else {
            p2.acceleration = DEFAULT_ACCELERATION;
        }
    }

    void Verlet() {
        for (auto &p : particles) {
            PVector &x = p.position;
            PVector temp = x;
            PVector &old_x = p.old_position;
            PVector a = p.acceleration; // acceleration const for now
            x += x-old_x+a*fTimeStep*fTimeStep;
            old_x = temp;
        }
    }

    virtual void SatisfyConstraints() {
        for(int j=0; j < 5; j++) { // TODO variable num iterations
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
//            Particle &p1 = particles[0];
//            static PVector pinned_pos = {380, 225}; //TODO temp non static
//            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) pinned_pos = {GetMousePosition().x, GetMousePosition().y};
//            p1.position = pinned_pos;
        }
    }

    virtual void Draw() {
        for (auto &p : particles) {
            p.draw();
        }
        for (auto c : constraints) {
            DrawLine(particles[c.particle_a].position.x, particles[c.particle_a].position.y, particles[c.particle_b].position.x,
                     particles[c.particle_b].position.y, WHITE);
        }
    }

    void AddParticle(Particle p) {
        particles.emplace_back(p);
    }

    void AddConstraint(Constraint c) {
        constraints.emplace_back(c);
    }

    struct Particle
    {
        PVector position;
        PVector old_position;
        float radius;
        float inv_mass;
        PVector acceleration = DEFAULT_ACCELERATION;

        void draw() const {
            DrawCircle(position.x, position.y, radius, WHITE);
        }
    };

    struct Constraint
    {
        int particle_a;
        int particle_b;
        float length;

    };

};


class Square : public ParticleSystem {
public:
    Square(PVector origin, float width, float height) {
        this->AddParticle({origin, origin, 4, DEFAULT_MASS, DEFAULT_ACCELERATION});
        this->AddParticle({{origin.x + width, origin.y}, {origin.x + width, origin.y}, 4, DEFAULT_MASS, DEFAULT_ACCELERATION});
        this->AddParticle({{origin.x + width, origin.y + height}, {origin.x + width, origin.y + height}, 4, DEFAULT_MASS, DEFAULT_ACCELERATION});
        this->AddParticle({{origin.x, origin.y + height}, {origin.x, origin.y + height}, 4, DEFAULT_MASS, DEFAULT_ACCELERATION});

        this->AddConstraint({0,1, width});
        this->AddConstraint({1,2, height});
        this->AddConstraint({2,3, width});
        this->AddConstraint({3,0, height});
        this->AddConstraint({0,2, sqrtf(width*width+height*height)});
        this->AddConstraint({1,3, sqrtf(width*width+height*height)});
    }

    ~Square() override = default;
};


class Rope : public ParticleSystem {
private:
    PVector origin_;

public:
    Rope(PVector origin, float length, int slices) : origin_{origin} {
        float slice_s = length/static_cast<float>(slices);
        for (int i = 0; i < slices; i++) {
            this->AddParticle(Particle{{origin.x, origin.y + static_cast<float>(i) * slice_s},
                                       {origin.x, origin.y + static_cast<float>(i) * slice_s},
                                       4, DEFAULT_MASS});
            if (i >= 1) {
                this->AddConstraint({i, i-1, slice_s});
            }
        }
    }

    ~Rope() override = default;


    void SatisfyConstraints() override {
        for(int j=0; j < 10; j++) { // TODO variable num iterations
            for (auto &c : constraints) {
                PVector& x1 = particles[c.particle_a].position;
                PVector& x2 = particles[c.particle_b].position;
                PVector delta = x2 - x1;
                float deltalength = sqrt(delta.dot(delta));
                float diff = (deltalength-c.length)
                             /(deltalength*(particles[c.particle_a].inv_mass+particles[c.particle_b].inv_mass));
                x1 += delta*diff*particles[c.particle_a].inv_mass;
                x2 -= delta*diff*particles[c.particle_b].inv_mass;
            }
            // TODO temp attaching origin_ to mouse coords
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) origin_ = {GetMousePosition().x, GetMousePosition().y};
            particles[0].position = origin_;
        }
    }

    void Draw() override {
        for (auto &p : particles) {
            p.draw();
        }
        DrawCurve();
    }

    void DrawCurve()
    {
        if (particles.size() < 2) return;

        PVector p0 = particles[0].position;
        PVector p1 = particles[1].position;
        PVector c1 = {(p0.x + p1.x) / 2.0f, (p0.y + p1.y) / 2.0f };
        DrawLineBezierQuad({static_cast<float>(p0.x), static_cast<float>(p0.y)},
                           {static_cast<float>(c1.x), static_cast<float>(c1.y)},
                           {static_cast<float>(p0.x), static_cast<float>(p0.y)},
                           4, WHITE);

        for (int i = 0; i < particles.size() - 2; i++)
        {
            p0 = particles[i].position;
            p1 = particles[i + 1].position;
            PVector p2 = particles[i + 2].position;

            c1 = { (p0.x + p1.x) / 2.0f, (p0.y + p1.y) / 2.0f };
            PVector c2 = {(p1.x + p2.x) / 2.0f, (p1.y + p2.y) / 2.0f };

            DrawLineBezierQuad({static_cast<float>(c1.x), static_cast<float>(c1.y)},
                               {static_cast<float>(c2.x), static_cast<float>(c2.y)},
                               {static_cast<float>(p1.x), static_cast<float>(p1.y)},
                               4, WHITE);
        }

        int n = particles.size();
        PVector pn1 = particles[n - 2].position;
        PVector pn = particles[n - 1].position;
        PVector cn1 = {(pn1.x + pn.x) / 2.0f, (pn1.y + pn.y) / 2.0f };
        DrawLineBezierQuad({static_cast<float>(cn1.x), static_cast<float>(cn1.y)},
                           {static_cast<float>(pn.x), static_cast<float>(pn.y)},
                           {static_cast<float>(pn.x), static_cast<float>(pn.y)},
                           4, WHITE);
    }

};


#endif //PROCEDURALANIMATION_PARTICLESYSTEM_H
