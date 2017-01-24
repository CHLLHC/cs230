#include <map>
#include <sstream>
#include <string>
#include "boolean.h"
#include "flat_shader.h"
#include "phong_shader.h"
#include "plane.h"
#include "point_light.h"
#include "reflective_shader.h"
#include "render_world.h"
#include "sphere.h"

void Parse(Render_World& world,int& width,int& height,const char* test_file)
{
    FILE* F = fopen(test_file,"r");
    if(!F)
    {
        printf("Failed to open file '%s'\n",test_file);
        exit(EXIT_FAILURE);
    }

    double f0;
    char buff[1000];
    vec3 u,v,w;
    std::string name,s0,s1,s2;
    
    std::map<std::string,vec3> colors;
    std::map<std::string,Object*> objects;
    std::map<std::string,Shader*> shaders;

    while(fgets(buff, sizeof(buff), F))
    {
        std::stringstream ss(buff);
        std::string item,name;
        if(!(ss>>item) || !item.size() || item[0]=='#') continue;
        if(item=="size")
        {
            ss>>width>>height;
            assert(ss);
        }
        else if(item=="color")
        {
            ss>>name>>u;
            assert(ss);
            colors[name]=u;
        }
        else if(item=="plane")
        {
            ss>>name>>u>>v>>s0;
            assert(ss);
            Object* o=new Plane(u,v);
            std::map<std::string,Shader*>::const_iterator sh=shaders.find(s0);
            assert(sh!=shaders.end());
            o->material_shader=sh->second;
            if(name=="-") world.objects.push_back(o);
            else objects[name]=o;
        }
        else if(item=="sphere")
        {
            ss>>name>>u>>f0>>s0;
            assert(ss);
            Object* o=new Sphere(u,f0);
            std::map<std::string,Shader*>::const_iterator sh=shaders.find(s0);
            assert(sh!=shaders.end());
            o->material_shader=sh->second;
            if(name=="-") world.objects.push_back(o);
            else objects[name]=o;
        }
        else if(item=="intersection" || item=="union" || item=="difference")
        {
            ss>>name>>s0>>s1>>s2;
            assert(ss);
            std::map<std::string,Object*>::const_iterator o0=objects.find(s0);
            std::map<std::string,Object*>::const_iterator o1=objects.find(s1);
            assert(o0!=objects.end());
            assert(o1!=objects.end());
            Boolean::Type type;
            if(item=="intersection") type=Boolean::type_intersection;
            else if(item=="union") type=Boolean::type_union;
            else type=Boolean::type_difference;
            Object* o=new Boolean(o0->second,o1->second,type);
            std::map<std::string,Shader*>::const_iterator sh=shaders.find(s2);
            assert(sh!=shaders.end());
            o->material_shader=sh->second;
            if(name=="-") world.objects.push_back(o);
            else objects[name]=o;
        }
        else if(item=="flat_shader")
        {
            ss>>name>>s0;
            assert(ss);
            std::map<std::string,vec3>::const_iterator c0=colors.find(s0);
            assert(c0!=colors.end());
            shaders[name]=new Flat_Shader(world,c0->second);
        }
        else if(item=="phong_shader")
        {
            ss>>name>>s0>>s1>>s2>>f0;
            assert(ss);
            std::map<std::string,vec3>::const_iterator c0=colors.find(s0);
            std::map<std::string,vec3>::const_iterator c1=colors.find(s1);
            std::map<std::string,vec3>::const_iterator c2=colors.find(s2);
            assert(c0!=colors.end());
            assert(c1!=colors.end());
            assert(c2!=colors.end());
            shaders[name]=new Phong_Shader(world,c0->second,c1->second,c2->second,f0);
        }
        else if(item=="reflective_shader")
        {
            ss>>name>>s0>>f0;
            assert(ss);
            std::map<std::string,Shader*>::const_iterator sh=shaders.find(s0);
            assert(sh!=shaders.end());
            shaders[name]=new Reflective_Shader(world,sh->second,f0);
        }
        else if(item=="point_light")
        {
            ss>>u>>s0>>f0;
            assert(ss);
            std::map<std::string,vec3>::const_iterator c0=colors.find(s0);
            assert(c0!=colors.end());
            world.lights.push_back(new Point_Light(u,c0->second,f0));
        }
        else if(item=="ambient_light")
        {
            ss>>s0>>f0;
            assert(ss);
            std::map<std::string,vec3>::const_iterator c0=colors.find(s0);
            assert(c0!=colors.end());
            world.ambient_color=c0->second;
            world.ambient_intensity=f0;
        }
        else if(item=="camera")
        {
            ss>>u>>v>>w>>f0;
            assert(ss);
            world.camera.Position_And_Aim_Camera(u,v,w);
            world.camera.Focus_Camera(1,(double)width/height,f0*(M_PI/180));
        }
        else if(item=="background")
        {
            ss>>s0;
            assert(ss);
            std::map<std::string,Shader*>::const_iterator sh=shaders.find(s0);
            assert(sh!=shaders.end());
            world.background_shader=sh->second;
        }
        else if(item=="enable_shadows")
        {
            ss>>world.enable_shadows;
            assert(ss);
        }
        else if(item=="recursion_depth_limit")
        {
            ss>>world.recursion_depth_limit;
            assert(ss);
        }
        else
        {
            std::cout<<"Failed to parse: "<<buff<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
    if(!world.background_shader)
        world.background_shader=new Flat_Shader(world,vec3());
    world.camera.Set_Resolution(ivec2(width,height));
}
