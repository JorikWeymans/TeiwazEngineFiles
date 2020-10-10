# TeiwazEngineFiles
 Copy of files from my Teiwaz engine which are intresting to look at
 
 
# Design choices

## Singletons
I am not really a fan of singletons, I have the feeling that  I don't have a lot of control over it. 
I try yo avoid them everywhere I can but for contentmanger I make an exception.
I use something like gamecontext which is made before the engine runs, and I give it to the scene
the scene, gameobject and Component are able to access this pointer.
This is a singleton but not a static variable. which gives me more control (or at least I think so)

## String over wstring
Because the scene is saved in binary format, writing both string and wstring is hard.
if wfstream is used instead of normal stream you alsways save 2(wchar_t) bytes instead of 1.
This gave a lot of porblems when writing int (some ints do not work) and you are also unable to save a byte.
for these 2 reason I made sure that every string saved in a component is a normal one so it can be read easly.

## SceneObject and Transform component
Every SceneObject can have several types of components and duplicate components, not every component needs to be on the sceneObject.
This is different for the Transform component, every sceneobject has a unique transform component.
This component is stored seperatly. I did this because 99% of all the objects needs a transform and also because this component is referenced by other components a lot it makes sence to store this on his own (other components can still be found but these arr all in an array and take a bit longer to find).

## Physics
The physics is done in 2 ways, There is a function that every component can call called Raycast, this will cast a ray from a position in a direction with a certain length if you hit something you get the position and the object you hit back. There is also an update that happens every FixedUpdate, this update goes over all the dynamic colliders and sees if it checks with either a static or another dynamic, if a collider is hit a function (which you can subscribe to in the same fashion as Unity Events) gets called. Dynamic collider: This collider is made for moving objects, when dynamic you can use OnColliderHit function Static collider: used for non-moving objects.

## Animators
The animator works like it does in Unity, giving it is a much more barebone system. When making  an animator you can give connections to it, each connection connects one animations to another and it has a condition (greater, equal, ..) for either a bool or a string.

## Binary files
There are a lot of binary files each with its own suffix. The suffix is mostly for keeping things clean, to avoid confusion if a file is an animation or a scene. That is also why at the start of the file the engine writes a hashed string, it uses this as an identifier when it cannot read this it will not read the rest of the file, this is done to prevent garbage data. The Binary files are not durable, one-byte writing to much or little the whole file becomes unreadable, and it is hard to fix. But this is the nature of Binary files I guess.

## Why DirectX
The main reason DirectX was chosen for this project is so post-processing effects could be used, the DirectX library has options for PP but I did not find the time to add it, yet.

## Prefabs
Something I forgot for most of the development but I used it a bit, The prefabs are not SceneObjects they are merely static classes with a create function, this made it easier To generate objects via code (Because Scene is final and SceneObject is final, it is hard to find a way of adding objects via code). Prefabs were perfect for this. I would like to have it in the editor in the future

## Magic Enums
Because there are a lot of enums that need to be visible in the editor and they always keep expanding (like componenType) I wanted something that could convert my enum values into strings automatically (like in Unity). Magic enums does this, it is a single .h include and free to use. At the start, it worked fine but sometimes I noticed that values started leaking into each other.
Example: enum fruit{ Apple, Kiwi, Orange} if I did enum_name(fruit::Apple) I would get string = "aplleKiwi"
I don't know how this happened, it does not work well with bitwise enums, I will probably not use it in the next project.


## FixedUpdate and Update
Every Decent Engine should have at least 2 update loops. I did one that runs as fast as it can and a FixedUpdate, the fixedUpdate is used for physics (that includes character movement). This is a much more consistent loop (50 updates a sec).

## ImGui
ImGUI is used in this project, it is an amazing library, I love it. I Compiled the ImGui files into a static library and compiled it with my graphics library, that is why every ImGui function is called SDXL_ImGUI.

## static vs Dynamic library
For an engine, the best library would be a dynamic library. This allows you to change the engine without needing to recompile the game. I did this for the graphics library, which allowed me to change that library and just copy over the .lib. For my engine, however, I did not do it, because dynamic libraries are a pain to work with especially while developing. During development, I like to use a static library because it requires less effort with the downside that it is not standalone. 

## CMBase templated class (ContentManger Base)
This class is something I made kind of late in development but it makes expanding and debugging so much easier. The engine has Textures, Animations, Scenes, Animators, ... . These things all need to be displayed in several ways: on The tabs below and in the contentmanager window. I found myself copying over a lot of code and that is when I decided to make this class. I don't even know why I mentioned it, maybe because it is one of my more inventive template classes or maybe because after writing it I felt so good. But anyway, I like it :)

## EDITOR_MODE
As I already mentioned I created an editor. This is defined with a macro EDITOR_MODE, before I called it that it was called USE_IM_GUI. The reason it was called that is because I just wanted to have a few things displayed in ImGUi Windows. But that quickly became a fully functional editor, which I enjoyed creating but it took so much of my time. Because the editor was not an idea from the start I am not really a fan of how it looks (in code) a lot of classes have ifdef statements. I also created editor only classes, which made it more clear and made everything more defined. But in the end, I still have the feeling that you can see that an editor mode was never intended. If I would do this again, I would first sit down and think how I am going to connect the editor mode with the non-editor mode

## Final thoughts
I had a lot of fun creating this, but because I didn't plan some things well I began procrastinating a bit. I also spent to much time on small details in the editor (which I tend to do) and not focussing on the big picture, of the creating of the game in that matter.
In the end, I also had some structural problems that were hard to fix, or to find that were created in the first few days, I would rewrite the whole sceneManager again before I do anything to the engine itself. 
With new projects of this scope, I am first going to make the foundation, the basics of the engine (loops, sceneManager, contentManager) is sturdy to prevent bugs so I would not regret it weeks or even months later.
