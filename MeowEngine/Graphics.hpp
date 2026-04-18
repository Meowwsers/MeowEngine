//
// Created by Stěpán Toman on 17.04.2026.
//

#ifndef MEOWENGINE_GRAPHICS_HPP
#define MEOWENGINE_GRAPHICS_HPP

#include "Core.hpp"
#include "Application.hpp"
#include "Player.hpp"
#include "Math.hpp"

class Graphics {
public:
    explicit Graphics(Application& application1) : application(application1) {}
    Application& application;

    void clearBackground(Color color) {
        SDL_SetRenderDrawColor(application.renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(application.renderer);
    }

    void update() {
        SDL_RenderPresent(application.renderer);
    }

    void drawLine(Line line, int thickness, Color color) {
        // Calculate line direction
        float dx = line.endPos.x - line.startPos.x;
        float dy = line.endPos.y - line.startPos.y;
        float len = std::sqrt(dx * dx + dy * dy);
        
        if (len == 0) return; // Degenerate line
        
        // Normalize direction
        dx /= len;
        dy /= len;
        
        // Calculate perpendicular direction
        float px = -dy;
        float py = dx;
        
        // Offset for thickness
        float offset = thickness / 2.0f;
        
        // Normalize color from 0-255 to 0.0-1.0
        SDL_FColor fcolor = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
        
        // Create vertices for a quad (thick line)
        SDL_Vertex vertices[4] = {
            {{line.startPos.x + px * offset, line.startPos.y + py * offset}, fcolor, {0.0f, 0.0f}},
            {{line.startPos.x - px * offset, line.startPos.y - py * offset}, fcolor, {0.0f, 0.0f}},
            {{line.endPos.x - px * offset, line.endPos.y - py * offset}, fcolor, {0.0f, 0.0f}},
            {{line.endPos.x + px * offset, line.endPos.y + py * offset}, fcolor, {0.0f, 0.0f}}
        };
        
        int indices[6] = {0, 1, 2, 0, 2, 3};
        
        SDL_RenderGeometry(application.renderer, NULL, vertices, 4, indices, 6);
    }

    void drawRect(Rect rect, Color color) {
        SDL_SetRenderDrawColor(application.renderer, color.r, color.g, color.b, color.a);
        SDL_FRect fRect = {rect.pos.x, rect.pos.y, rect.size.x, rect.size.y};
        SDL_RenderFillRect(application.renderer, &fRect);
    }

    void drawPlayer(const Player& player) {
        drawRect(player.rect, player.color);
    }

    void drawRectOutline(Rect rect, Color color, int thickness) {
        // Efficient single-call rectangle outline using SDL_RenderGeometry
        float halfThickness = thickness / 2.0f;
        SDL_FColor fcolor = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
        
        // Outer corners
        float2 tl = {rect.pos.x, rect.pos.y};
        float2 tr = {rect.pos.x + rect.size.x, rect.pos.y};
        float2 br = {rect.pos.x + rect.size.x, rect.pos.y + rect.size.y};
        float2 bl = {rect.pos.x, rect.pos.y + rect.size.y};
        
        // Inner corners (offset by thickness)
        float2 tl_in = {rect.pos.x + halfThickness, rect.pos.y + halfThickness};
        float2 tr_in = {rect.pos.x + rect.size.x - halfThickness, rect.pos.y + halfThickness};
        float2 br_in = {rect.pos.x + rect.size.x - halfThickness, rect.pos.y + rect.size.y - halfThickness};
        float2 bl_in = {rect.pos.x + halfThickness, rect.pos.y + rect.size.y - halfThickness};
        
        // Create 8 vertices (outer 4 + inner 4)
        SDL_Vertex vertices[8] = {
            {{tl.x, tl.y}, fcolor, {0.0f, 0.0f}},           // 0: outer top-left
            {{tr.x, tr.y}, fcolor, {0.0f, 0.0f}},           // 1: outer top-right
            {{br.x, br.y}, fcolor, {0.0f, 0.0f}},           // 2: outer bottom-right
            {{bl.x, bl.y}, fcolor, {0.0f, 0.0f}},           // 3: outer bottom-left
            {{tl_in.x, tl_in.y}, fcolor, {0.0f, 0.0f}},     // 4: inner top-left
            {{tr_in.x, tr_in.y}, fcolor, {0.0f, 0.0f}},     // 5: inner top-right
            {{br_in.x, br_in.y}, fcolor, {0.0f, 0.0f}},     // 6: inner bottom-right
            {{bl_in.x, bl_in.y}, fcolor, {0.0f, 0.0f}}      // 7: inner bottom-left
        };
        
        // Create indices for the rectangle outline (quad strip as triangles)
        int indices[24] = {
            // Top edge
            0, 4, 1,
            1, 4, 5,
            // Right edge
            1, 5, 2,
            2, 5, 6,
            // Bottom edge
            2, 6, 3,
            3, 6, 7,
            // Left edge
            3, 7, 0,
            0, 7, 4
        };
        
        SDL_RenderGeometry(application.renderer, NULL, vertices, 8, indices, 24);
    }

    void drawRoundedRectOutline(Rect rect, Color color, int thickness, float cornerRadius) {
        // Draw rect outline using 4 lines
        float2 topLeft = {rect.pos.x, rect.pos.y};
        float2 topRight = {rect.pos.x + rect.size.x, rect.pos.y};
        float2 bottomRight = {rect.pos.x + rect.size.x, rect.pos.y + rect.size.y};
        float2 bottomLeft = {rect.pos.x, rect.pos.y + rect.size.y};
        
        // Draw the 4 lines
        Line top = {topLeft, topRight};
        Line right = {topRight, bottomRight};
        Line bottom = {bottomRight, bottomLeft};
        Line left = {bottomLeft, topLeft};
        
        drawLine(top, thickness, color);
        drawLine(right, thickness, color);
        drawLine(bottom, thickness, color);
        drawLine(left, thickness, color);
        
        // Draw circles at corners to create rounded effect
        drawCircle(topLeft, cornerRadius, color);
        drawCircle(topRight, cornerRadius, color);
        drawCircle(bottomRight, cornerRadius, color);
        drawCircle(bottomLeft, cornerRadius, color);
    }

    void drawCircle(float2 center, float radius, Color color) {
        const int segments = 32; // Number of segments for the circle
        const float pi = 3.14159265359f;
        
        std::vector<SDL_Vertex> vertices;
        std::vector<int> indices;
        
        // Add center vertex
        vertices.push_back({
            {center.x, center.y},
            {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
            {0.0f, 0.0f}
        });
        
        // Add circle perimeter vertices
        for (int i = 0; i <= segments; i++) {
            float angle = (2.0f * pi * i) / segments;
            float x = center.x + radius * std::cos(angle);
            float y = center.y + radius * std::sin(angle);
            
            vertices.push_back({
                {x, y},
                {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
                {0.0f, 0.0f}
            });
        }
        
        // Create indices for triangles from center to perimeter
        for (int i = 1; i <= segments; i++) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        
        SDL_RenderGeometry(application.renderer, NULL, vertices.data(), vertices.size(), indices.data(), indices.size());
    }

    void drawTriangle(Triangle tri, Color color) {
        SDL_FColor fcolor = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
        
        SDL_Vertex vertices[3] = {
            {{tri.a.x, tri.a.y}, fcolor, {0.0f, 0.0f}},
            {{tri.b.x, tri.b.y}, fcolor, {0.0f, 0.0f}},
            {{tri.c.x, tri.c.y}, fcolor, {0.0f, 0.0f}}
        };
        
        int indices[3] = {0, 1, 2};
        
        SDL_RenderGeometry(application.renderer, NULL, vertices, 3, indices, 3);
    }

    void drawTriangleOutline(Triangle tri, Color color, int thickness) {
        SDL_FColor fcolor = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
        
        // Helper to compute inward normal for each edge
        auto edgeVertices = [&](float2 p1, float2 p2, float2 opposite) -> std::array<SDL_Vertex, 4> {
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float len = std::sqrt(dx * dx + dy * dy);
            dx /= len; dy /= len;
            
            // Perpendicular pointing inward (toward opposite vertex)
            float nx = -dy, ny = dx;
            float mx = opposite.x - p1.x, my = opposite.y - p1.y;
            if (nx * mx + ny * my < 0) { nx = -nx; ny = -ny; }
            
            return {{
                {{p1.x, p1.y}, fcolor, {0.0f, 0.0f}},
                {{p1.x + nx * thickness, p1.y + ny * thickness}, fcolor, {0.0f, 0.0f}},
                {{p2.x + nx * thickness, p2.y + ny * thickness}, fcolor, {0.0f, 0.0f}},
                {{p2.x, p2.y}, fcolor, {0.0f, 0.0f}}
            }};
        };
        
        auto drawEdge = [&](float2 p1, float2 p2, float2 opposite) {
            auto v = edgeVertices(p1, p2, opposite);
            int indices[6] = {0, 1, 2, 0, 2, 3};
            SDL_RenderGeometry(application.renderer, NULL, v.data(), 4, indices, 6);
        };
        
        drawEdge(tri.a, tri.b, tri.c);
        drawEdge(tri.b, tri.c, tri.a);
        drawEdge(tri.c, tri.a, tri.b);
    }

    void drawCircleOutline(float2 center, float radius, int thickness, Color color) {
        const int segments = 32; // Number of segments for the circle
        const float pi = 3.14159265359f;
        
        // Draw circle outline using lines
        for (int i = 0; i < segments; i++) {
            float angle1 = (2.0f * pi * i) / segments;
            float angle2 = (2.0f * pi * (i + 1)) / segments;
            
            float x1 = center.x + radius * std::cos(angle1);
            float y1 = center.y + radius * std::sin(angle1);
            float x2 = center.x + radius * std::cos(angle2);
            float y2 = center.y + radius * std::sin(angle2);
            
            Line segment = {{x1, y1}, {x2, y2}};
            drawLine(segment, thickness, color);
        }
    }

    void drawBezier(const Curve& curve, Color color, int thickness, float flatnessTolerance = 1.0f) {
        std::vector<float2> points;
        points.push_back(curve.startPoint);

        collectBezierPoints(
            curve.startPoint,
            curve.controlPoint1,
            curve.controlPoint2,
            curve.endPoint,
            flatnessTolerance,
            0,
            points
        );

        drawPolyline(points, color, thickness);
    }

private:
    void collectBezierPoints(
        float2 p0, float2 p1, float2 p2, float2 p3,
        float tol, int depth, std::vector<float2>& points
    ) {
        auto distToLine = [](float2 a, float2 b, float2 p) -> float {
            const float dx = b.x - a.x;
            const float dy = b.y - a.y;
            const float len2 = dx * dx + dy * dy;
            if (len2 < 1e-6f) {
                return std::sqrt((p.x - a.x) * (p.x - a.x) + (p.y - a.y) * (p.y - a.y));
            }
            const float t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / len2;
            const float cx = a.x + t * dx - p.x;
            const float cy = a.y + t * dy - p.y;
            return std::sqrt(cx * cx + cy * cy);
        };

        const float d1 = distToLine(p0, p3, p1);
        const float d2 = distToLine(p0, p3, p2);

        if ((d1 + d2) <= tol || depth >= 16) {
            points.push_back(p3);
            return;
        }

        // De Casteljau split at t = 0.5
        const float2 m01  = { (p0.x + p1.x) * 0.5f, (p0.y + p1.y) * 0.5f };
        const float2 m12  = { (p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f };
        const float2 m23  = { (p2.x + p3.x) * 0.5f, (p2.y + p3.y) * 0.5f };
        const float2 m012 = { (m01.x + m12.x) * 0.5f, (m01.y + m12.y) * 0.5f };
        const float2 m123 = { (m12.x + m23.x) * 0.5f, (m12.y + m23.y) * 0.5f };
        const float2 mid  = { (m012.x + m123.x) * 0.5f, (m012.y + m123.y) * 0.5f };

        collectBezierPoints(p0, m01, m012, mid, tol, depth + 1, points);
        collectBezierPoints(mid, m123, m23, p3, tol, depth + 1, points);
    }

    void drawPolyline(const std::vector<float2>& points, Color color, int thickness) {
        if (points.size() < 2 || thickness <= 0) {
            return;
        }

        const float halfThickness = thickness * 0.5f;
        const SDL_FColor fcolor = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};

        auto normalize = [](float2 value) -> float2 {
            const float len = std::sqrt(value.x * value.x + value.y * value.y);
            if (len <= 0.0001f) {
                return {0.0f, 0.0f};
            }
            return {value.x / len, value.y / len};
        };

        auto perpendicular = [](float2 value) -> float2 {
            return {-value.y, value.x};
        };

        auto dot = [](float2 a, float2 b) -> float {
            return a.x * b.x + a.y * b.y;
        };

        std::vector<SDL_Vertex> vertices;
        vertices.reserve(points.size() * 2);

        for (size_t i = 0; i < points.size(); i++) {
            float2 joinNormal;

            if (i == 0) {
                const float2 dir = normalize({points[1].x - points[0].x, points[1].y - points[0].y});
                joinNormal = perpendicular(dir);
            } else if (i == points.size() - 1) {
                const float2 dir = normalize({points[i].x - points[i - 1].x, points[i].y - points[i - 1].y});
                joinNormal = perpendicular(dir);
            } else {
                const float2 prevDir = normalize({points[i].x - points[i - 1].x, points[i].y - points[i - 1].y});
                const float2 nextDir = normalize({points[i + 1].x - points[i].x, points[i + 1].y - points[i].y});
                const float2 prevNormal = perpendicular(prevDir);
                const float2 nextNormal = perpendicular(nextDir);
                const float2 miter = normalize({prevNormal.x + nextNormal.x, prevNormal.y + nextNormal.y});

                if (miter.x == 0.0f && miter.y == 0.0f) {
                    joinNormal = nextNormal;
                } else {
                    const float projection = std::max(0.2f, dot(miter, nextNormal));
                    joinNormal = {miter.x / projection, miter.y / projection};
                }
            }

            const float2 offset = {joinNormal.x * halfThickness, joinNormal.y * halfThickness};
            vertices.push_back({{points[i].x + offset.x, points[i].y + offset.y}, fcolor, {0.0f, 0.0f}});
            vertices.push_back({{points[i].x - offset.x, points[i].y - offset.y}, fcolor, {0.0f, 0.0f}});
        }

        std::vector<int> indices;
        indices.reserve((points.size() - 1) * 6);
        for (size_t i = 0; i + 1 < points.size(); i++) {
            const int base = static_cast<int>(i * 2);
            indices.push_back(base);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 1);
            indices.push_back(base + 3);
            indices.push_back(base + 2);
        }

        SDL_RenderGeometry(application.renderer, nullptr, vertices.data(), static_cast<int>(vertices.size()), indices.data(), static_cast<int>(indices.size()));
    }
};

#endif //MEOWENGINE_GRAPHICS_HPP
