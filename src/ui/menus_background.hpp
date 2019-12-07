#include <common.hpp>
#include <components.hpp>
#include <effect_manager.hpp>

class MenuBackground
{

public:
    void init(vec2 screen_size);

    void destroy();

    void draw(const mat3& projection);

    vec2 get_position();

    vec2 get_bounding_box();

private:
    MeshComponent mesh{};
    Effect effect{};
    Transform transform;
    Texture menubgSprite;
    mat3 out;
};
