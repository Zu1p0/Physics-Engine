#pragma once

#include <SDL.h>
#include <vector>
#include <iostream>
#include <variant>

#include "Element.h"
#include "vec2.h"

class Solver
{
public:
    Solver(SDL_Renderer* renderer) : renderer(renderer) {};

    void update() {
		const double dt = calculate_dt();
        const double step_dt = calculate_step_dt(dt);

        for (uint32_t i{ m_sub_steps }; i--;) {
            apply_gravity();
			apply_collisions(step_dt);
            apply_constraint();
            update_objects(step_dt);
        }
    }

    void render() {
        for (Element& circle : m_objects) {
            circle.render(renderer);
        }
    }

	void set_sub_steps(uint32_t sub_steps) { 
        m_sub_steps = sub_steps; 
    }

    void set_gravity(vec2f gravity) {
        m_gravity = gravity;
    }

    void add_object(vec2f pos, int r, SDL_Color col) {
        m_objects.emplace_back(Element(pos, r, col));
    }

    const std::vector<Element>& get_objects() const {
        return m_objects;
    }

    void add_constraint(std::variant<Circle, Rect> constraint)
    {
		m_constraints.emplace_back(constraint);
    }

private:
    SDL_Renderer* renderer;

    uint32_t m_sub_steps = 1;
    vec2f m_gravity = { 0, 0 };

    std::vector<std::variant<Circle, Rect>> m_constraints;

    std::vector<Element> m_objects;

    Uint64 m_now = SDL_GetPerformanceCounter();
    Uint64 m_last = 0;

    void apply_gravity() {
        for (Element& obj: m_objects) {
			obj.accelerate(m_gravity);
        }
    }

    double calculate_dt() {
        m_last = m_now;
        m_now = SDL_GetPerformanceCounter();

        return (double)((m_now - m_last) / (double)SDL_GetPerformanceFrequency());
    }

    double calculate_step_dt(double dt) const {
        return dt / m_sub_steps;
    }

    void update_objects(double step_dt) {
        for (Element& obj : m_objects) {
			obj.update(step_dt);
        }
    }

    void apply_collisions(float step_dt) {
        const float response_coef = 0.75;
		const uint64_t object_count = m_objects.size();

        for (uint64_t i = 0; i < object_count; i++) {
			Element& obj1 = m_objects[i];
            
            for (uint64_t j = i + 1; j < object_count; j++) {
                Element& obj2 = m_objects[j];
                
                vec2f v = obj1.pos - obj2.pos;
                const float dist2 = v.x * v.x + v.y * v.y;
                const float min_dist = obj1.radius + obj2.radius;

                if (dist2 < min_dist * min_dist) {
                    float dist = sqrt(dist2);

                    vec2f n = (vec2f)(v / dist);
                    const float mass_ratio_1 = obj1.radius / (static_cast<float>(obj1.radius) + obj2.radius);
                    const float mass_ratio_2 = obj2.radius / (static_cast<float>(obj1.radius) + obj2.radius);
                    const float delta = 0.5f * response_coef * (dist - min_dist);

                    vec2f temp1 = (n * (mass_ratio_2 * delta));
                    obj1.pos -= temp1;

                    vec2f temp2 = (n * (mass_ratio_1 * delta));
                    obj2.pos += temp2;
                }
            }
        }
    }

    void apply_constraint()
    {
        for (auto& constraint : m_constraints) {
            if (std::holds_alternative<Circle>(constraint)) {
                for (Element& obj : m_objects) {
                    vec2f v = std::get<Circle>(constraint).center - obj.pos;
                    double dist = sqrt(v.x * v.x + v.y * v.y);
                    if (dist > (std::get<Circle>(constraint).radius - obj.radius)) {
                        vec2f n = v / dist;
                        vec2f temp = (n * (std::get<Circle>(constraint).radius - obj.radius));
                        obj.pos = (std::get<Circle>(constraint).center - temp);
                    }
                }
            }
        }
    }
};