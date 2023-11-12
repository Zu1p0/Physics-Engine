#pragma once

#include <SDL.h>
#include <iostream>

#include "vec2.h"

typedef struct Circle {
    
	vec2f center;
    float radius;

    float x = center.x;
    float y = center.y;
    float r = radius;
};

typedef struct Rect {
    float x;
    float y;
    float width;
    float height;

    float w = width;
    float h = height;

    vec2f center = { x + (width / 2), y + (height / 2)};
};

class Element
{
public:
    Element(vec2f pos, int r, SDL_Color col) : pos(pos), pos_last(pos), radius(r), color(col) {}

    void update(float dt)
    {
        // Compute how much we moved
        vec2f displacement = pos - pos_last;
        // Update position
        pos_last = pos;
        
        vec2f delta_acceleration = (acceleration * (dt * dt));
        pos = pos + displacement + delta_acceleration;

        // Reset acceleration
        acceleration = {};
    }

    void accelerate(vec2f a)
    {
        acceleration += a;
    }

    void set_velocity(vec2f v, float dt)
    {
        vec2f delta_vec = (v * dt);
        pos_last = pos - delta_vec;
    }

    void add_velocity(vec2f v, float dt)
    {
        vec2f delta_vec = (v * dt);
        pos -= delta_vec;
    }

    vec2f get_velocity(float dt)
    {
        return (pos - pos_last) / dt;
    }

	void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        int offsetx, offsety, d;

        offsetx = 0;
        offsety = radius;
        d = radius - 1;

        while (offsety >= offsetx) {
            SDL_RenderDrawLine(renderer, pos.x - offsety, pos.y + offsetx,
                pos.x + offsety, pos.y + offsetx);
            SDL_RenderDrawLine(renderer, pos.x - offsetx, pos.y + offsety,
                pos.x + offsetx, pos.y + offsety);
            SDL_RenderDrawLine(renderer, pos.x - offsetx, pos.y - offsety,
                pos.x + offsetx, pos.y - offsety);
            SDL_RenderDrawLine(renderer, pos.x - offsety, pos.y - offsetx,
                pos.x + offsety, pos.y - offsetx);

            if (d >= 2 * offsetx) {
                d -= 2 * offsetx + 1;
                offsetx += 1;
            }
            else if (d < 2 * (radius - offsety)) {
                d += 2 * offsety - 1;
                offsety -= 1;
            }
            else {
                d += 2 * (offsety - offsetx - 1);
                offsety -= 1;
                offsetx += 1;
            }
        }
	}
    
	int get_radius() const { return radius; }
	void set_radius(int r) { radius = r; }

	vec2f get_pos() const { return pos; }

	SDL_Color get_color() const { return color; }
	void set_color(SDL_Color c) { color = c; }
	void set_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { color = { r, g, b, a }; }
	
    vec2f pos;
    int radius;
private:
    
    vec2f pos_last;
    vec2f acceleration;     
	
	SDL_Color color;
};

