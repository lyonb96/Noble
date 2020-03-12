# Noble
Noble is a work-in-progress game engine that I've been developing to practice and demonstrate my skills with C++. It uses custom containers and strings, custom memory management tools, and more.

# Architecture
The gameplay-layer of the engine is built similar to Unreal Engine or Unity, where a Component system is used to build custom functionality. GameObjects are containers of Components, and Components provide the unique behavior for GameObjects. GameObjects can be possessed by Controllers, which provide input queues to move Objects through the world.

The lower-level architecture is fairly simple. The Engine class handles initializing subsystems like the logger, physics, renderer. Each of these subsystems are wrapped in their own class(es).

# To-do
Currently, I'm working on implementing Bullet physics into the engine. Once I feel comfortable with this, I want to move on to fleshing out rendering and implementing more than simple static meshes. At this point, I'd also like to build up ShaderTool to be easier to use. Additionally, I want to expand on the AssetManager to support packed assets, ie loading multiple assets from a single file. 

# Third-party
The engine uses BGFX for rendering, Bullet for physics, GLM for mathematics, and nlohmann's JSON parser for reading config files (though this may change to INI files at some point, I haven't decided).
