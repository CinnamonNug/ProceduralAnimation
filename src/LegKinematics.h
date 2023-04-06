//
// Created by Cengiz Cimen on 4/04/2023.
//

#ifndef PROCEDURALANIMATION_LEGKINEMATICS_H
#define PROCEDURALANIMATION_LEGKINEMATICS_H

#include <ParticleSystem.h>
#include <PVector.h>
#include "imgui/imgui.h"
#include "raylib/rlImGui.h"
#include <raylib.h>



class LegKinematics {
public:
    PVector targ1;
    PVector targ2;

    PVector hip{400, 400};
    PVector cent{400, 600};

    PVector foot0{0, 0};
    PVector knee0{50, 0};
    PVector foot1{0,0};
    PVector knee1{50,0};

    PVector floor1{200, 600};
    PVector floor2{600, 600};

    float gallop_height = 70, gallop_width = 150, gallop_speed = 7, leg_length = 200;
    float l = leg_length/2, hip_x = hip.x, hip_y = hip.y, gait_offset = 0, gallop_wave_frequency = 2;

    void Draw() const {
        ClearBackground(BLACK);

        DrawCircleP(targ1, 10, BLUE);
        DrawCircleP(targ2, 10, BLUE);
        DrawCircleP(cent, 5, YELLOW);
        DrawCircleP(hip, 5, YELLOW);

        DrawLineP(hip, knee0, 20, PURPLE);
        DrawLineP(knee0, foot0, 20, PURPLE);
        DrawLineP(hip, knee1, 20, GREEN);
        DrawLineP(knee1, foot1, 20,GREEN);

        DrawLineP(floor1, floor2, 10,GREEN);
    }

    void Update() {
        if (IsKeyDown(KEY_D)) {
            cent.x += 1 * gallop_speed;
            hip_x += 1 * gallop_speed;
        }
        if (IsKeyPressed(KEY_A)) {
            cent.x = 400;
            hip_x = 400;
        }
        l = leg_length/2; // Incase the leg length changes

        float t1 = GetTime() * gallop_speed + gait_offset;
        float t2 = GetTime() * gallop_speed;

        hip.x = hip_x; hip.y = hip_y;
        hip.y = hip.y + gallop_speed * sin(t1*gallop_wave_frequency) ;

        // Desired x and y of the hip
        targ1 = cent + PVector{gallop_width * cos(t1), gallop_height * sin(t1)};
        targ2 = cent + PVector{-gallop_width * cos(t2), gallop_height * sin(-t2)};

        targ2.y = hip.y + fmin(abs(hip.y - targ2.y), abs(hip.y - 600));
        targ1.y = hip.y + fmin(abs(hip.y - targ1.y), abs(hip.y - 600));

        //////////////
        // Calculations for leg 1
        PVector diff0 = targ1 - hip;
        float d0 = sqrt(diff0.x * diff0.x + diff0.y * diff0.y);
        d0 = std::fmin(d0, leg_length);

        float b0 = atan2(diff0.y, diff0.x);
        float a1 = acos((l*l + d0 * d0 - l * l) / (2 * l * d0));
        float a2 = acos((l*l + l*l - d0 * d0) / (2 * l * l));
        float hip_angle = b0 - a1;
        float knee_angle = PI - a2;

        knee0.x = hip.x + l * cos(hip_angle);
        knee0.y = hip.y + l * sin(hip_angle);

        foot0.x = knee0.x + l * cos(hip_angle + knee_angle);
        foot0.y = knee0.y + l * sin(hip_angle + knee_angle);

        // Calculations for leg 2
        PVector diff1 = targ2 - hip;
        float d1 = sqrt(diff1.x * diff1.x + diff1.y * diff1.y);
        d1 = std::fmin(d1, leg_length);

        float b1 = atan2(diff1.y, diff1.x);
        float a3 = acos((l*l + d1*d1 - l*l) / (2 * l * d1));
        float a4 = acos((l*l + l*l - d1*d1) / (2 * l * l));
        float hip_angle1 = b1 - a3;
        float knee_angle1 = PI - a4;

        knee1.x = hip.x + l * cos(hip_angle1);
        knee1.y = hip.y + l * sin(hip_angle1);

        foot1.x = knee1.x + l * cos(hip_angle1 + knee_angle1);
        foot1.y = knee1.y + l * sin(hip_angle1 + knee_angle1);
    }


    void DrawImgui() {
        rlImGuiBegin();
        if (ImGui::Begin("Main Window"), true) {
            ImGui::DragFloat("gallop_height", &gallop_height,
                             1.0f, 0, 200);
            ImGui::DragFloat("gallop_width", &gallop_width,
                             1.0f, 0, 200);
            ImGui::DragFloat("gallop_speed", &gallop_speed,
                             1.0f, 0, 200);
            ImGui::DragFloat("gallop_wave_frequency", &gallop_wave_frequency,
                             0.01f, 0, 3);
            ImGui::DragFloat("leg_length", &leg_length,
                             1.0f, 0, 300);
            ImGui::DragFloat("hip_x", &hip_x,
                             1.0f, -1000, 1000);
            ImGui::DragFloat("hip_y", &hip_y,
                             1.0f, -1000, 1000);
            ImGui::DragFloat("gait_offset", &gait_offset,
                             0.1f, -200, 200);

        }
        ImGui::End();
        rlImGuiEnd();
    }
};


#endif //PROCEDURALANIMATION_LEGKINEMATICS_H
