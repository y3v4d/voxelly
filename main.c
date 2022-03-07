#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <GL/glew.h>
#include <GL/glx.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

const char *vertex_shader_source = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main() {\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}";

const char *fragment_shader_source = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}";


int main() {
    Atom wm_delete_window;

    // open new display connection
    Display *display = XOpenDisplay(NULL);
    if(!display) {
        printf("Failed to open X display\n");
        exit(1);
    }

    int visual_attribs[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None
    };

    int screen = XDefaultScreen(display);

    printf("Getting matching framebuffer configs\n");
    int fb_count;
    GLXFBConfig *fbc = glXChooseFBConfig(display, screen, visual_attribs, &fb_count);
    if(!fbc) {
        printf("Failed to retrive framebuffer config\n");
        exit(1);
    }
    printf("Found %d matching configs\n", fb_count);

    int best_samples_num = -1, best_index = -1;
    for(int i = 0; i < fb_count; ++i) {
        XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
        if(vi) {
            int sample_buffers, samples;

            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &sample_buffers);
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

            printf("Matching framebuffer config %d, visual ID %ld: SAMPLE_BUFFERS = %d, SAMPLES = %d\n", i, vi->visualid, sample_buffers, samples);

            if(sample_buffers && samples > best_samples_num) {
                best_index = i;
                best_samples_num = samples;
            }
        }

        XFree(vi);
    }


    GLXFBConfig best_fb = fbc[best_index];
    XFree(fbc);

    XVisualInfo *vi = glXGetVisualFromFBConfig(display, best_fb);
    if(!vi) {
        printf("Couldn't get visual info from framebuffer config\n");
        exit(1);
    }

    printf("Chosen visual ID %ld\n", vi->visualid);

    // create window
    XSetWindowAttributes window_attributes;
    window_attributes.colormap = XCreateColormap(display, XRootWindow(display, vi->screen), vi->visual, AllocNone);
    window_attributes.background_pixel = XWhitePixel(display, vi->screen);
    window_attributes.border_pixel = XBlackPixel(display, vi->screen);

    Window window = XCreateWindow(
            display,                        // display
            XRootWindow(display, vi->screen),   // root window
            0, 0,                           // window position
            640, 480,                       // window size
            0,                              // border size
            vi->depth,                      // depth
            InputOutput,                    // window flags
            vi->visual,                     // visual
            CWBackPixel | CWBorderPixel | CWColormap,    // attributes flags
            &window_attributes              // window attributes
    );
    if(!window) {
        printf("Failed to create X window\n");
        exit(1);
    }

    XMapWindow(display, window);

    // free visual info
    XFree(vi);

    // set WM_DELETE_WINDOW atom
    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    // get ARB function to create modern opengl context in x11
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

    // create opengl context
    GLXContext context = 0;
    if(!glXCreateContextAttribsARB) {
        printf("Can't create modern OpenGL context, not supported\n");
        exit(1);
    } else {
        int context_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        };

        context = glXCreateContextAttribsARB(display, best_fb, 0, True, context_attribs);
        XSync(display, False);
        if(!context) {
            printf("Can't create modern OpenGL context\n");
            exit(1);
        }
    }

    glXMakeCurrent(display, window, context);

    glewExperimental = GL_TRUE;
    glewInit();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    const GLubyte *version = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", version);
    
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // event loop
    XEvent event;
    Bool done = False;
    while(!done) {
        while(XPending(display)) {
            XNextEvent(display, &event);
            if(event.type == ClientMessage) {
                if(event.xclient.data.l[0] == (long)wm_delete_window) {
                    printf("WM_DELETE_WINDOW invoked\n");
                    done = True;      
                }
            }
        }

        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glXSwapBuffers(display, window);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glXMakeCurrent(display, 0, 0);
    glXDestroyContext(display, context);

    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
