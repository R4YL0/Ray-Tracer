# Ray Tracer
This is an expanded Ray Tracer from [This Tutorial](https://raytracing.github.io/). As the tutorial ended with some bugs in the Ray Tracer, I'm still trying to fix them and add additional features like *Post-Processing Filters* and *OBJ Files support*.
## How to use it
Similar to the Tutorial, to change the output, most of the changes can be done in the *main.cc* File:
- You can choose between one of the 12 scenes (most are comming from the Tutorial)
- Each scene consists of following Attributes:
    - The first three link the scene to the *world*, *camera*, and *lights*
    - The 4th Attribute is the *Resolution*
    - The 5th Attribute is *Samples Per Pixel*
    - The 6th Attribute is *Bounce Depth* (meaning, how often the Ray bounces, important for Global illumination)
- Except the first three, all attributes have a *default* (see `scenes.h` to see their values)