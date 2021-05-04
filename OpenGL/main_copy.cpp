
#include "functions.h"

int main()
{
    // ****************** initialization process ****************************************
    // initialize glfw
    glfwInit();
    
    // set the window version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // for mac only, for crossplatform purpose
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // prevent the window from resizing itself
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    // create a window
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "COMP371 PA2", nullptr, nullptr);
    
    // for mac to adjust the window size
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    // check if the window has been created correctly
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    // initialize glew
    glewExperimental = true;
    if(GLEW_OK != glewInit())
    {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    
    
    // Compile and link shaders here ...
    Shader colorShader("resources/shaders/core.vs", "resources/shaders/core.fs");
    Shader textureShader("resources/shaders/texture.vs", "resources/shaders/texture.fs");
    
    // load texture
    GLuint boxTextureID = loadTexture("assets/textures/box.jpg");
    GLuint metalTextureID = loadTexture("assets/textures/metal.png");
    GLuint leatherTextureID = loadTexture("assets/textures/leather.png");
    
    // Define and upload geometry to the GPU here ...
    GLuint vbo = createVertexBufferObject();
    // ******************** end of system initializatoin ***************************************

    
    // *****************Variables declarations*******************
    float worldRotationAngleZ = 0.0f;
    float worldRotationAngleY = 0.0f;
    float lastFrameTime = glfwGetTime();    // time is in seconds
//    float movementSpeed = 0.5f;
    modelSelector ms;
    GLenum RENDER_MODE = GL_TRIANGLES;
    float p = (GRID_SCALE/2)*gridUnit;  // radius of imaginary circle
    float modelScale = 2.0f;            // model scaling
    float theta = 2.5* modelScale;      // model arc angle
    float initialEyeX = 0.0f;
    float initialEyeY = 30*gridUnit;
    float initialEyeZ = (GRID_SCALE/2+25) * gridUnit;
    glm::vec3 initialEye(initialEyeX, initialEyeY, initialEyeZ);

    double dx = 0.0f;
    double dy = 0.0f;
    float fov = 70.0f;
    double mousePosX, mousePosY, unusedMouse;
    double lastMousePosX = 0.0f, lastMousePosY = 0.0f;


    // ******************* set initial camera position *****************
    glm::vec3 eye = initialEye;
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), 1024.0f / 768.0f, 0.01f, 100.0f);
    glm::mat4 viewMatrix = glm::lookAt(eye,  // eye
        center,  // center
        cameraUp);

    setViewMatrix(colorShader.program, viewMatrix);
    setViewMatrix(textureShader.program, viewMatrix);

    setProjectionMatrix(colorShader.program, projectionMatrix);
    setProjectionMatrix(textureShader.program, projectionMatrix);
    
    //**************** end of initial camera position *******************************************
    

    // enable backface culling and depth test
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    
    
    // main game loop
    while(!glfwWindowShouldClose(window))
    {
        // detect inputs from keyboard
        glfwPollEvents();
        
        // set background color (r, g, b, alpha)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        // Each frame, reset color of each pixel to glClearColor and clear depth buffer bit
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vbo);

        // compute the rotational angle every loop.
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        
        
        
        // ***************** define the world matrix for world rotation**************
        glm::mat4 worldRotationY = glm::rotate(glm::mat4(1.0f), glm::radians(worldRotationAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 worldRotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(worldRotationAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));
        glm:: mat4 worldRotationMatrix = worldRotationZ * worldRotationY;

        
        // ************************ draw the 128 * 128 grid **************************************
        colorShader.use();
        GLuint worldMatrixLocation = glGetUniformLocation(colorShader.program, "worldMatrix");
        for (int i = 0; i <= GRID_SCALE; i++) {
            float basePos = -(GRID_SCALE / 2 * gridUnit);
            // to draw a grid, we need to scale, (rotate for z), and translate the unit line.
            // translation is calculated by loop variable
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, basePos + i * gridUnit));
            // rotation is the same
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            // scaling is the same, scale along x axis
            glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(-basePos, 0.0f, 0.0f));
            
            // for the lines parallel to x axis, no need for rotation
            glm::mat4 gridMatrix = worldRotationMatrix * translation * scaling;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gridMatrix[0][0]);
            glDrawArrays(GL_LINES, GRID_OFFSET, GRID_COUNT);
            
            // for the lines parallel to z axis, need rotation
            translation = glm::translate(glm::mat4(1.0f), glm::vec3(basePos+ i * gridUnit, 0.0f, 0.0f));
            gridMatrix = worldRotationMatrix * translation * rotation * scaling;
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &gridMatrix[0][0]);
            glDrawArrays(GL_LINES, GRID_OFFSET, GRID_COUNT);
        }
        
        // draw the x-y-z axis
        //************** axis matrix to rotate around with world*****************
        glm::mat4 axisMatrix = glm::mat4(1.0f);
        axisMatrix = axisMatrix * worldRotationMatrix;
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
        glDrawArrays(GL_LINES, AXIS_OFFSET, AXIS_COUNT);
        // ********************************************************************************
        
        
        
        // ************** Define 5 sets of model matrices **********************
        // ====== 1st set of model: JI40
        // ======== To be placed at front:
        // ======== Define parent matrix for this model
        glm::mat4 scalingParent1 = glm::scale(glm::mat4(1.0f),
                                              glm::vec3(ms.getModelScalingFactor(1), ms.getModelScalingFactor(1), ms.getModelScalingFactor(1)));
        glm::mat4 rotationParent1 = glm::rotate(glm::mat4(1.0f), glm::radians(ms.getModelRotationAngle(1)-25.0f), glm::vec3(0.0f, 1.0f, 0.0f));    // rotate about the y axis only
        glm::mat4 translationParent1 = glm::translate(glm::mat4(1.0f), ms.getModelTranslationPos(1) - glm::vec3(p-10*gridUnit, 0.0f, 20*gridUnit));
        glm::mat4 parent1 = worldRotationMatrix* translationParent1 * rotationParent1* scalingParent1;
        

        
        // ======== Letter J ========
        glm::mat4 JScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 JRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f + theta*3), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 JTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*sin(glm::radians(90.0f-3*theta)) + p, 6*gridUnit, p*cos(glm::radians(90.0f-3*theta))));
        glm::mat4 JMatrix = parent1 * JTranslateMatrix * JRotationMatrix * JScaleMatrix;
        
          //j the top stick
         glm::mat4 JTheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
         glm::mat4 JTheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
         glm::mat4 JTheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
         glm::mat4 JTheTopStickMatrix = JMatrix * JTheTopStickTranslateMatrix * JTheTopStickRotationMatrix * JTheTopStickScaleMatrix;
          
          //J the right stick
         glm::mat4 JTheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
         glm::mat4 JTheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 0.5f, .0f));
         glm::mat4 JTheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
         glm::mat4 JTheRightStickMatrix = JMatrix * JTheRightStickTranslateMatrix * JTheRightStickRotationMatrix * JTheRightStickScaleMatrix;
         
         //J the left stick
         glm::mat4 JTheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 1.0f));
         glm::mat4 JTheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
         glm::mat4 JTheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -2.0f, 0.0f));
         glm::mat4 JTheLeftStickMatrix = JMatrix * JTheLeftStickTranslateMatrix * JTheLeftStickRotationMatrix * JTheLeftStickScaleMatrix;
          
          //J the bottom stick
         glm::mat4 JTheBotStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
         glm::mat4 JTheBotStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
         glm::mat4 JTheBotStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.5f, 0.0f));
         glm::mat4 JTheBotStickMatrix = JMatrix * JTheBotStickTranslateMatrix * JTheBotStickRotationMatrix * JTheBotStickScaleMatrix;
        
        
        
        //Letter i==========================================================
        
        glm::mat4 iScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 iRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f + theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 iTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*sin(glm::radians(90.0f-theta)) + p, 6*gridUnit, p*cos(glm::radians(90.0f-theta))));
        glm::mat4 iMatrix = parent1 * iTranslateMatrix * iRotationMatrix * iScaleMatrix;
        
        //iTheDot
        glm::mat4 iTheDotScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
        glm::mat4 iTheDotRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 iTheDotTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f));
        glm::mat4 iTheDotMatrix = iMatrix * iTheDotTranslateMatrix * iTheDotRotationMatrix * iTheDotScaleMatrix;
       
        //iTheStick
        glm::mat4 iTheStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
        glm::mat4 iTheStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 iTheStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
        glm::mat4 iTheStickMatrix = iMatrix * iTheStickTranslateMatrix * iTheStickRotationMatrix * iTheStickScaleMatrix;
        
        
        
        
        //Number 4_1==============================================================
        glm::mat4 number4ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 number4RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f - theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number4TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*sin(glm::radians(90.0f-theta)) + p,6*gridUnit, -p*cos(glm::radians(90.0f-theta))));
        glm::mat4 number4Matrix = parent1 * number4TranslateMatrix * number4RotationMatrix * number4ScaleMatrix;
        
        //number 4 left stick
                
        glm::mat4 number4TheLeftStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f)));
        glm::mat4 number4TheLeftStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4TheLeftStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(-2.0f, 1.5f, 0.0f)));
        glm::mat4 number4TheLeftStickMatrix = number4Matrix * number4TheLeftStickTranslateMatrix * number4TheLeftStickRotationMatrix * number4TheLeftStickScaleMatrix;

        //number 4 mid stick
       
        glm::mat4 number4TheMidStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f)));
        glm::mat4 number4TheMidStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        glm::mat4 number4TheMidStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f)));
        glm::mat4 number4TheMidStickMatrix = number4Matrix * number4TheMidStickTranslateMatrix * number4TheMidStickRotationMatrix * number4TheMidStickScaleMatrix;

        //number 4 Right stick
               
        glm::mat4 number4TheRightStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f)));
        glm::mat4 number4TheRightStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4TheRightStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        glm::mat4 number4TheRightStickMatrix = number4Matrix * number4TheRightStickTranslateMatrix * number4TheRightStickRotationMatrix * number4TheRightStickScaleMatrix;

        
        
        //Number 0_1=============================================================
        glm::mat4 number0ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 number0RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f-3*theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number0TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*sin(glm::radians(90.0f-3*theta)) + p, 6*gridUnit, -p*cos(glm::radians(90.0f-3*theta))));
        glm::mat4 number0Matrix = parent1 * number0TranslateMatrix * number0RotationMatrix * number0ScaleMatrix;
        
        
        //0 the left stick
             
        glm::mat4 number0TheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 number0TheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 number0TheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
        glm::mat4 number0TheLeftStickMatrix = number0Matrix * number0TheLeftStickTranslateMatrix * number0TheLeftStickRotationMatrix * number0TheLeftStickScaleMatrix;
                
        //0 the top stick
        glm::mat4 number0TheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number0TheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number0TheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, 2.5f, 0.0f));
        glm::mat4 number0TheTopStickMatrix = number0Matrix * number0TheTopStickTranslateMatrix * number0TheTopStickRotationMatrix * number0TheTopStickScaleMatrix;
                
        //0 the right stick
               
        glm::mat4 number0TheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 number0TheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 0.5f, .0f));
        glm::mat4 number0TheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));
        glm::mat4 number0TheRightStickMatrix = number0Matrix * number0TheRightStickTranslateMatrix * number0TheRightStickRotationMatrix * number0TheRightStickScaleMatrix;
                
        //0 the bottom stick
              
        glm::mat4 number0TheBotStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number0TheBotStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number0TheBotStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, 0.0f));
        glm::mat4 number0TheBotStickMatrix = number0Matrix * number0TheBotStickTranslateMatrix * number0TheBotStickRotationMatrix * number0TheBotStickScaleMatrix;
                

        
        // ====== 2nd set of model: LE48 ===================================================
        // ======== To be placed at front:
        // ======== Define parent matrix for this model
        glm::mat4 scalingParent2 = glm::scale(glm::mat4(1.0f),
                                              glm::vec3(ms.getModelScalingFactor(2), ms.getModelScalingFactor(2), ms.getModelScalingFactor(2)));
        glm::mat4 rotationParent2 = glm::rotate(glm::mat4(1.0f), glm::radians(ms.getModelRotationAngle(2)), glm::vec3(0.0f, 1.0f, 0.0f));    // rotate about the y axis only
        glm::mat4 translationParent2 = glm::translate(glm::mat4(1.0f), ms.getModelTranslationPos(2) + glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 parent2 = worldRotationMatrix * translationParent2 * rotationParent2 * scalingParent2;
        
        
        //Letter L=======================================================================
        glm::mat4 LScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 LRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f + theta*3), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 LTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(p*cos(glm::radians(90.0f-3*theta)),6*gridUnit, p*sin(glm::radians(90.0f-3*theta)) - p));
        glm::mat4 LMatrix = parent2 * LTranslateMatrix * LRotationMatrix * LScaleMatrix;
        
     
        //L the left stick
           
        glm::mat4 LTheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 LTheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 LTheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
        glm::mat4 LTheLeftStickMatrix = LMatrix * LTheLeftStickTranslateMatrix * LTheLeftStickRotationMatrix * LTheLeftStickScaleMatrix;
                            
        //L the bottom stick
                
        glm::mat4 LTheBotStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 LTheBotStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 LTheBotStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.5f, 0.0f));
        glm::mat4 LTheBotStickMatrix = LMatrix * LTheBotStickTranslateMatrix * LTheBotStickRotationMatrix * LTheBotStickScaleMatrix;
                 
        //Letter E =======================================================================
        glm::mat4 EScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 ERotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f + theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 ETranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(p*cos(glm::radians(90.0f-theta)),6*gridUnit, p*sin(glm::radians(90.0f-theta)) - p));
        glm::mat4 EMatrix = parent2 * ETranslateMatrix * ERotationMatrix * EScaleMatrix;
        
             
        //E the top stick
            
        glm::mat4 ETheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 ETheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 ETheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.5f, 0.0f));
        glm::mat4 ETheTopStickMatrix = EMatrix * ETheTopStickTranslateMatrix * ETheTopStickRotationMatrix * ETheTopStickScaleMatrix;
              
        //E the left stick
           
        glm::mat4 ETheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 ETheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 ETheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
        glm::mat4 ETheLeftStickMatrix = EMatrix * ETheLeftStickTranslateMatrix * ETheLeftStickRotationMatrix * ETheLeftStickScaleMatrix;
             
        //E the mid stick
            
        glm::mat4 ETheMidStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 ETheMidStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 ETheMidStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
        glm::mat4 ETheMidStickMatrix = EMatrix * ETheMidStickTranslateMatrix * ETheMidStickRotationMatrix * ETheMidStickScaleMatrix;
         
        //E the bottom stick
           
        glm::mat4 ETheBotStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 ETheBotStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 ETheBotStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.5f, 0.0f));
        glm::mat4 ETheBotStickMatrix = EMatrix * ETheBotStickTranslateMatrix * ETheBotStickRotationMatrix * ETheBotStickScaleMatrix;
        
        //Number 4_2 ==============================================================
        glm::mat4 number4_2ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 number4_2RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f-theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number4_2TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*cos(glm::radians(90.0f-theta)),6*gridUnit, p*sin(glm::radians(90.0f-theta)) - p));
        glm::mat4 number4_2Matrix = parent2 * number4_2TranslateMatrix * number4_2RotationMatrix * number4_2ScaleMatrix;
        
        //number 4 left stick
                
        glm::mat4 number4_2TheLeftStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f)));
        glm::mat4 number4_2TheLeftStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4_2TheLeftStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(-2.0f, 1.5f, 0.0f)));
        glm::mat4 number4_2TheLeftStickMatrix = number4_2Matrix * number4_2TheLeftStickTranslateMatrix * number4_2TheLeftStickRotationMatrix * number4_2TheLeftStickScaleMatrix;

        
        //number 4 mid stick
       
        glm::mat4 number4_2TheMidStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f)));
        glm::mat4 number4_2TheMidStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        glm::mat4 number4_2TheMidStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f)));
        glm::mat4 number4_2TheMidStickMatrix = number4_2Matrix * number4_2TheMidStickTranslateMatrix * number4_2TheMidStickRotationMatrix * number4_2TheMidStickScaleMatrix;

        //number 4 Right stick
               
        glm::mat4 number4_2TheRightStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f)));
        glm::mat4 number4_2TheRightStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4_2TheRightStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        glm::mat4 number4_2TheRightStickMatrix = number4_2Matrix * number4_2TheRightStickTranslateMatrix * number4_2TheRightStickRotationMatrix * number4_2TheRightStickScaleMatrix;
        
        
        
        //Number 8==============================================================
        glm::mat4 number8ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 number8RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f-3*theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number8TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*cos(glm::radians(90.0f-3*theta)),6*gridUnit, p*sin(glm::radians(90.0f-3*theta)) - p));
        glm::mat4 number8Matrix = parent2 * number8TranslateMatrix * number8RotationMatrix * number8ScaleMatrix;
               
       
        //8 the left stick
              
        glm::mat4 number8TheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 number8TheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 number8TheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
        glm::mat4 number8TheLeftStickMatrix = number8Matrix * number8TheLeftStickTranslateMatrix * number8TheLeftStickRotationMatrix * number8TheLeftStickScaleMatrix;
        //8 the top stick
               
        glm::mat4 number8TheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number8TheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number8TheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
        glm::mat4 number8TheTopStickMatrix = number8Matrix * number8TheTopStickTranslateMatrix * number8TheTopStickRotationMatrix * number8TheTopStickScaleMatrix;
                       
        //8 the right stick
      
        glm::mat4 number8TheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 number8TheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 number8TheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));
        glm::mat4 number8TheRightStickMatrix = number8Matrix * number8TheRightStickTranslateMatrix * number8TheRightStickRotationMatrix * number8TheRightStickScaleMatrix;
               
               
        //8 the mid stick
           
        glm::mat4 number8TheMidStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number8TheMidStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number8TheMidStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 number8TheMidStickMatrix = number8Matrix * number8TheMidStickTranslateMatrix * number8TheMidStickRotationMatrix * number8TheMidStickScaleMatrix;
               
                   
        //8 the bottom stick
              
        glm::mat4 number8TheBotStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number8TheBotStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number8TheBotStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, 0.0f));
        glm::mat4 number8TheBotStickMatrix = number8Matrix * number8TheBotStickTranslateMatrix * number8TheBotStickRotationMatrix * number8TheBotStickScaleMatrix;
               

        
        
        
        
        // ====== 3rd set of model: ZN46
        // To be placed at bottom:
   
        glm::mat4 scalingParent3 = glm::scale(glm::mat4(1.0f),
            glm::vec3(ms.getModelScalingFactor(3), ms.getModelScalingFactor(3), ms.getModelScalingFactor(3)));
        glm::mat4 rotationParent3 = glm::rotate(glm::mat4(1.0f), glm::radians(ms.getModelRotationAngle(3)+25.0f), glm::vec3(0.0f, 1.0f, 0.0f));    // rotate about the y axis only
        glm::mat4 translationParent3 = glm::translate(glm::mat4(1.0f), ms.getModelTranslationPos(3) + glm::vec3(p-10*gridUnit,0.0f,-20*gridUnit));
        glm::mat4 parent3 = worldRotationMatrix * translationParent3 * rotationParent3* scalingParent3;
        

        // ======== Letter Z ========
        glm::mat4 ZScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.6 * gridUnit, 1.6 * gridUnit, 1.6 * gridUnit));
        glm::mat4 ZRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f - theta * 3), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 ZTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(p * sin(glm::radians(90.0f - 3 * theta)) - p, 6 * gridUnit, p * cos(glm::radians(90.0f - 3 * theta))));
        glm::mat4 ZMatrix = parent3 * ZTranslateMatrix * ZRotationMatrix * ZScaleMatrix;

        //Z the top stick
        glm::mat4 ZTheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 ZTheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 ZTheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
        glm::mat4 ZTheTopStickMatrix = ZMatrix * ZTheTopStickTranslateMatrix * ZTheTopStickRotationMatrix * ZTheTopStickScaleMatrix;

        //Z the middle stick
        glm::mat4 ZTheMidStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 8.0f, 1.0f));
        glm::mat4 ZTheMidStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(130.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 ZTheMidStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 ZTheMidStickMatrix = ZMatrix * ZTheMidStickTranslateMatrix * ZTheMidStickRotationMatrix * ZTheMidStickScaleMatrix;

        //Z the bottom stick
        glm::mat4 ZTheBotStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 ZTheBotStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 ZTheBotStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, 0.0f));
        glm::mat4 ZTheBotStickMatrix = ZMatrix * ZTheBotStickTranslateMatrix * ZTheBotStickRotationMatrix * ZTheBotStickScaleMatrix;


        //======== Letter N ========
        glm::mat4 NScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2 * gridUnit, 2 * gridUnit, 2 * gridUnit));
        glm::mat4 NRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f - theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 NTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(p * sin(glm::radians(90.0f -  theta)) - p, 6 * gridUnit, p * cos(glm::radians(90.0f - theta))));
        glm::mat4 NMatrix = parent3 * NTranslateMatrix * NRotationMatrix * NScaleMatrix;

        //N the Left stick
        glm::mat4 NTheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
        glm::mat4 NTheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 NTheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.75f, 0.0f, 0.0f));
        glm::mat4 NTheLeftStickMatrix = NMatrix * NTheLeftStickTranslateMatrix * NTheLeftStickRotationMatrix * NTheLeftStickScaleMatrix;

        //N the middle stick
        glm::mat4 NTheMidStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 NTheMidStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(35.0f), glm::vec3(.0f, 0.0f, 1.0f));
        glm::mat4 NTheMidStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 NTheMidStickMatrix = NMatrix * NTheMidStickTranslateMatrix * NTheMidStickRotationMatrix * NTheMidStickScaleMatrix;

        //N the right stick
        glm::mat4 NTheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
        glm::mat4 NTheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 NTheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.75f, 0.0f, 0.0f));
        glm::mat4 NTheRightStickMatrix = NMatrix * NTheRightStickTranslateMatrix * NTheRightStickRotationMatrix * NTheRightStickScaleMatrix;



        //Number 4_3==============================================================
        glm::mat4 number4_3ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale * gridUnit, modelScale * gridUnit, modelScale * gridUnit));
        glm::mat4 number4_3RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f + theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number4_3TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(p * sin(glm::radians(90.0f - theta)) - p, 6 * gridUnit, -p * cos(glm::radians(90.0f - theta))));
        glm::mat4 number4_3Matrix = parent3 * number4_3TranslateMatrix * number4_3RotationMatrix * number4_3ScaleMatrix;

        //number 4 left stick

        glm::mat4 number4_3TheLeftStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f)));
        glm::mat4 number4_3TheLeftStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4_3TheLeftStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(-2.0f, 1.5f, 0.0f)));
        glm::mat4 number4_3TheLeftStickMatrix = number4_3Matrix * number4_3TheLeftStickTranslateMatrix * number4_3TheLeftStickRotationMatrix * number4_3TheLeftStickScaleMatrix;

        //number 4 mid stick

        glm::mat4 number4_3TheMidStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f)));
        glm::mat4 number4_3TheMidStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        glm::mat4 number4_3TheMidStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f)));
        glm::mat4 number4_3TheMidStickMatrix = number4_3Matrix * number4_3TheMidStickTranslateMatrix * number4_3TheMidStickRotationMatrix * number4_3TheMidStickScaleMatrix;

        //number 4 Right stick

        glm::mat4 number4_3TheRightStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f)));
        glm::mat4 number4_3TheRightStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4_3TheRightStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        glm::mat4 number4_3TheRightStickMatrix = number4_3Matrix * number4_3TheRightStickTranslateMatrix * number4_3TheRightStickRotationMatrix * number4_3TheRightStickScaleMatrix;

        //======Number 6=========
        glm::mat4 number6ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale * gridUnit, modelScale * gridUnit, modelScale * gridUnit));
        glm::mat4 number6RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f + theta * 3), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number6TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(p * sin(glm::radians(90.0f - 3 * theta)) - p, 6 * gridUnit, -p * cos(glm::radians(90.0f - theta * 3))));
        glm::mat4 number6Matrix = parent3 * number6TranslateMatrix * number6RotationMatrix * number6ScaleMatrix;


        //6 the left stick

        glm::mat4 number6TheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 number6TheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 number6TheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
        glm::mat4 number6TheLeftStickMatrix = number6Matrix * number6TheLeftStickTranslateMatrix * number6TheLeftStickRotationMatrix * number6TheLeftStickScaleMatrix;
        //6 the top stick

        glm::mat4 number6TheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number6TheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number6TheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
        glm::mat4 number6TheTopStickMatrix = number6Matrix * number6TheTopStickTranslateMatrix * number6TheTopStickRotationMatrix * number6TheTopStickScaleMatrix;

        //6 the right stick

        glm::mat4 number6TheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number6TheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 number6TheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, -1.0f, 0.0f));
        glm::mat4 number6TheRightStickMatrix = number6Matrix * number6TheRightStickTranslateMatrix * number6TheRightStickRotationMatrix * number6TheRightStickScaleMatrix;


        //6 the mid stick

        glm::mat4 number6TheMidStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number6TheMidStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number6TheMidStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 number6TheMidStickMatrix = number6Matrix * number6TheMidStickTranslateMatrix * number6TheMidStickRotationMatrix * number6TheMidStickScaleMatrix;


        //6 the bottom stick

        glm::mat4 number6TheBotStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number6TheBotStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number6TheBotStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, 0.0f));
        glm::mat4 number6TheBotStickMatrix = number6Matrix * number6TheBotStickTranslateMatrix * number6TheBotStickRotationMatrix * number6TheBotStickScaleMatrix;

        
      
        
        // ====== 4st set of model: YN40=======================================
        // ======== Define parent matrix for this model
        glm::mat4 scalingParent4 = glm::scale(glm::mat4(1.0f),
                                              glm::vec3(ms.getModelScalingFactor(4), ms.getModelScalingFactor(4), ms.getModelScalingFactor(4)));
        glm::mat4 rotationParent4 = glm::rotate(glm::mat4(1.0f), glm::radians(ms.getModelRotationAngle(4)+45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // rotate about the y axis only
        glm::mat4 translationParent4 = glm::translate(glm::mat4(1.0f), ms.getModelTranslationPos(4) - glm::vec3(p-25*gridUnit, 0.0f,-20*gridUnit));
        glm::mat4 parent4 = worldRotationMatrix * translationParent4 * rotationParent4 * scalingParent4;
        
        //Letter Y
        glm::mat4 YScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 YRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f + theta*3), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 YTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*sin(glm::radians(90.0f-3*theta)) + p, 6*gridUnit, p*cos(glm::radians(90.0f-3*theta))));
        glm::mat4 YMatrix = parent4 * YTranslateMatrix * YRotationMatrix * YScaleMatrix;

        
        //Y The Left Stick
        glm::mat4 YTheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
        glm::mat4 YTheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 YTheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.5f, 0.0f));
        glm::mat4 YTheLeftStickMatrix = YMatrix * YTheLeftStickTranslateMatrix * YTheLeftStickRotationMatrix * YTheLeftStickScaleMatrix;
        
        //y The Middle Stick
        glm::mat4 YTheMidStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 5.0f, 1.0f));
        glm::mat4 YTheMidStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 YTheMidStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        glm::mat4 YTheMidStickMatrix = YMatrix * YTheMidStickTranslateMatrix * YTheMidStickRotationMatrix * YTheMidStickScaleMatrix;
        //y The Right Stick
               
        glm::mat4 YTheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f));
        glm::mat4 YTheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(130.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 YTheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.5f, 0.0f));
        glm::mat4 YTheRightStickMatrix = YMatrix * YTheRightStickTranslateMatrix * YTheRightStickRotationMatrix * YTheRightStickScaleMatrix;
        
        
        
        //Letter N========================================
        glm::mat4 n_2ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 n_2RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f + theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 n_2TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*sin(glm::radians(90.0f-theta)) + p, 6*gridUnit, p*cos(glm::radians(90.0f-theta))));
        glm::mat4 n_2Matrix = parent4 * n_2TranslateMatrix * n_2RotationMatrix * n_2ScaleMatrix;

        
        //n The Left Stick
              
        glm::mat4 n_2TheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 n_2TheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 n_2TheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
        glm::mat4 n_2TheLeftStickMatrix = n_2Matrix * n_2TheLeftStickTranslateMatrix * n_2TheLeftStickRotationMatrix * n_2TheLeftStickScaleMatrix;
        
        //n The Middle Stick
        glm::mat4 n_2TheMidStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.5f, 1.0f));
        glm::mat4 n_2TheMidStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 n_2TheMidStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 n_2TheMidStickMatrix = n_2Matrix * n_2TheMidStickTranslateMatrix * n_2TheMidStickRotationMatrix * n_2TheMidStickScaleMatrix;
        
               //n The Right Stick
             
        glm::mat4 n_2TheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 n_2TheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 n_2TheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
        glm::mat4 n_2TheRightStickMatrix = n_2Matrix * n_2TheRightStickTranslateMatrix * n_2TheRightStickRotationMatrix * n_2TheRightStickScaleMatrix;

        //Number 4_4 ==============================================================
        glm::mat4 number4_4ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 number4_4RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f - theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number4_4TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*sin(glm::radians(90.0f-theta)) + p,6*gridUnit, -p*cos(glm::radians(90.0f-theta))));
        glm::mat4 number4_4Matrix = parent4 * number4_4TranslateMatrix * number4_4RotationMatrix * number4_4ScaleMatrix;
        
        
        //number 4 left stick
                
        glm::mat4 number4_4TheLeftStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f)));
        glm::mat4 number4_4TheLeftStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4_4TheLeftStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(-2.0f, 1.5f, 0.0f)));
        glm::mat4 number4_4TheLeftStickMatrix = number4_4Matrix * number4_4TheLeftStickTranslateMatrix * number4_4TheLeftStickRotationMatrix * number4_4TheLeftStickScaleMatrix;

        
        //number 4 mid stick
       
        glm::mat4 number4_4TheMidStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f)));
        glm::mat4 number4_4TheMidStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        glm::mat4 number4_4TheMidStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f)));
        glm::mat4 number4_4TheMidStickMatrix = number4_4Matrix * number4_4TheMidStickTranslateMatrix * number4_4TheMidStickRotationMatrix * number4_4TheMidStickScaleMatrix;

        //number 4 Right stick
               
        glm::mat4 number4_4TheRightStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f)));
        glm::mat4 number4_4TheRightStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4_4TheRightStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        glm::mat4 number4_4TheRightStickMatrix = number4_4Matrix * number4_4TheRightStickTranslateMatrix * number4_4TheRightStickRotationMatrix * number4_4TheRightStickScaleMatrix;
        
        
        //Number 0_2=============================================================
        glm::mat4 number0_2ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 number0_2RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f-3*theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number0_2TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-p*sin(glm::radians(90.0f-3*theta)) + p, 6*gridUnit, -p*cos(glm::radians(90.0f-3*theta))));
        glm::mat4 number0_2Matrix = parent4 * number0_2TranslateMatrix * number0_2RotationMatrix * number0_2ScaleMatrix;
        
      
        //0 the left stick
             
        glm::mat4 number0_2TheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 number0_2TheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 number0_2TheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
        glm::mat4 number0_2TheLeftStickMatrix = number0_2Matrix * number0_2TheLeftStickTranslateMatrix * number0_2TheLeftStickRotationMatrix * number0_2TheLeftStickScaleMatrix;
                
        //0 the top stick
        glm::mat4 number0_2TheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number0_2TheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number0_2TheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, 2.5f, 0.0f));
        glm::mat4 number0_2TheTopStickMatrix = number0_2Matrix * number0_2TheTopStickTranslateMatrix * number0_2TheTopStickRotationMatrix * number0_2TheTopStickScaleMatrix;
                
        //0 the right stick
               
        glm::mat4 number0_2TheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 number0_2TheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 0.5f, .0f));
        glm::mat4 number0_2TheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));
        glm::mat4 number0_2TheRightStickMatrix = number0_2Matrix * number0_2TheRightStickTranslateMatrix * number0_2TheRightStickRotationMatrix * number0_2TheRightStickScaleMatrix;
                
        //0 the bottom stick
              
        glm::mat4 number0_2TheBotStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number0_2TheBotStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number0_2TheBotStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, 0.0f));
        glm::mat4 number0_2TheBotStickMatrix = number0_2Matrix * number0_2TheBotStickTranslateMatrix * number0_2TheBotStickRotationMatrix * number0_2TheBotStickScaleMatrix;
                
        
             
        // ====== 5th set of model:FG47====================================================
        // To be placed at middle:
        
        glm::mat4 scalingParent5 = glm::scale(glm::mat4(1.0f),
                                              glm::vec3(ms.getModelScalingFactor(5), ms.getModelScalingFactor(5), ms.getModelScalingFactor(5)));
        glm::mat4 rotationParent5 = glm::rotate(glm::mat4(1.0f), glm::radians(ms.getModelRotationAngle(5)-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // rotate about the y axis only
        glm::mat4 translationParent5 = glm::translate(glm::mat4(1.0f), ms.getModelTranslationPos(5) + glm::vec3(p-25*gridUnit,0.0f,15*gridUnit));
        glm::mat4 parent5 = worldRotationMatrix * translationParent5*rotationParent5 *  scalingParent5;
        
        
        
        //Letter F
        
        glm::mat4 FScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 FRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f - theta * 3), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 FTranslateMatrix =  glm::translate(glm::mat4(1.0f), glm::vec3(p * sin(glm::radians(90.0f - 3 * theta)) - p, 6 * gridUnit, p * cos(glm::radians(90.0f - 3 * theta))));
        glm::mat4 FMatrix = parent5 * FTranslateMatrix * FRotationMatrix * FScaleMatrix;
        
        

        //F the top stick
        glm::mat4 FTheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 FTheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 FTheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.5f, 0.0f));
        glm::mat4 FTheTopStickMatrix = FMatrix * FTheTopStickTranslateMatrix * FTheTopStickRotationMatrix * FTheTopStickScaleMatrix;
               
              //F the left stick
        glm::mat4 FTheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 FTheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 FTheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
        glm::mat4 FTheLeftStickMatrix = FMatrix * FTheLeftStickTranslateMatrix * FTheLeftStickRotationMatrix * FTheLeftStickScaleMatrix;
              
              //F the mid stick
             
        glm::mat4 FTheMidStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 FTheMidStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 FTheMidStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
        glm::mat4 FTheMidStickMatrix = FMatrix * FTheMidStickTranslateMatrix * FTheMidStickRotationMatrix * FTheMidStickScaleMatrix;
              

        //Letter G========================================================
        
        glm::mat4 GScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 GRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f - theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 GTranslateMatrix =  glm::translate(glm::mat4(1.0f), glm::vec3(p * sin(glm::radians(90.0f -  theta)) - p, 6 * gridUnit, p * cos(glm::radians(90.0f - theta))));
        glm::mat4 GMatrix = parent5 * GTranslateMatrix * GRotationMatrix * GScaleMatrix;
        

        //G the left stick
              
        glm::mat4 GTheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.5f, 1.0f));
        glm::mat4 GTheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, -1.0f, .0f));
        glm::mat4 GTheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -2.0f, 0.0f));
        glm::mat4 GTheLeftStickMatrix = GMatrix * GTheLeftStickTranslateMatrix * GTheLeftStickRotationMatrix * GTheLeftStickScaleMatrix;
               
        //G the upper left stick
             
        glm::mat4 GTheTopLStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 GTheTopLStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 GTheTopLStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 1.0f, 0.0f));
        glm::mat4 GTheTopLStickMatrix = GMatrix * GTheTopLStickTranslateMatrix * GTheTopLStickRotationMatrix * GTheTopLStickScaleMatrix;
               
        //G the top stick
               
        glm::mat4 GTheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 GTheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 GTheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.5f, 0.0f));
        glm::mat4 GTheTopStickMatrix = GMatrix * GTheTopStickTranslateMatrix * GTheTopStickRotationMatrix * GTheTopStickScaleMatrix;
               
               
        //G the right stick
             
        glm::mat4 GTheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 GTheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 1.0f, .0f));
        glm::mat4 GTheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 GTheRightStickMatrix = GMatrix * GTheRightStickTranslateMatrix * GTheRightStickRotationMatrix * GTheRightStickScaleMatrix;
               
               
        //G the mid stick
              
        glm::mat4 GTheMidStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 GTheMidStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 GTheMidStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
        glm::mat4 GTheMidStickMatrix = GMatrix * GTheMidStickTranslateMatrix * GTheMidStickRotationMatrix * GTheMidStickScaleMatrix;
               
                   
        //G the bottom stick
              
        glm::mat4 GTheBotStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 GTheBotStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 GTheBotStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.5f, 0.0f));
        glm::mat4 GTheBotStickMatrix = GMatrix * GTheBotStickTranslateMatrix * GTheBotStickRotationMatrix * GTheBotStickScaleMatrix;
               
        
        //Number 4_5 ==============================================================
        glm::mat4 number4_5ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 number4_5RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f+theta), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number4_5TranslateMatrix =glm::translate(glm::mat4(1.0f), glm::vec3(p * sin(glm::radians(90.0f - theta)) - p, 6 * gridUnit, -p * cos(glm::radians(90.0f - theta))));
        glm::mat4 number4_5Matrix = parent5 * number4_5TranslateMatrix * number4_5RotationMatrix * number4_5ScaleMatrix;
        
        
        
        //number 4 left stick
                
        glm::mat4 number4_5TheLeftStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f)));
        glm::mat4 number4_5TheLeftStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4_5TheLeftStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(-2.0f, 1.5f, 0.0f)));
        glm::mat4 number4_5TheLeftStickMatrix = number4_5Matrix * number4_5TheLeftStickTranslateMatrix * number4_5TheLeftStickRotationMatrix * number4_5TheLeftStickScaleMatrix;


        
        //number 4 mid stick
       
        glm::mat4 number4_5TheMidStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 4.0f, 1.0f)));
        glm::mat4 number4_5TheMidStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        glm::mat4 number4_5TheMidStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f)));
        glm::mat4 number4_5TheMidStickMatrix = number4_5Matrix * number4_5TheMidStickTranslateMatrix * number4_5TheMidStickRotationMatrix * number4_5TheMidStickScaleMatrix;

        //number 4 Right stick
               
        glm::mat4 number4_5TheRightStickScaleMatrix = glm::mat4(scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f)));
        glm::mat4 number4_5TheRightStickRotationMatrix = glm::mat4(rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::mat4 number4_5TheRightStickTranslateMatrix = glm::mat4(translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        glm::mat4 number4_5TheRightStickMatrix = number4_5Matrix * number4_5TheRightStickTranslateMatrix * number4_5TheRightStickRotationMatrix * number4_5TheRightStickScaleMatrix;

        //Number 7=======================================================
        glm::mat4 number7ScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(modelScale*gridUnit, modelScale*gridUnit, modelScale*gridUnit));
        glm::mat4 number7RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f + theta * 3), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 number7TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(p * sin(glm::radians(90.0f - 3 * theta)) - p, 6 * gridUnit, -p * cos(glm::radians(90.0f - theta * 3))));
        glm::mat4 number7Matrix = parent5 * number7TranslateMatrix * number7RotationMatrix * number7ScaleMatrix;
        
        //7 the top stick
               
        glm::mat4 number7TheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 1.0f));
        glm::mat4 number7TheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 number7TheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.5f, 0.0f));
        glm::mat4 number7TheTopStickMatrix = number7Matrix * number7TheTopStickTranslateMatrix * number7TheTopStickRotationMatrix * number7TheTopStickScaleMatrix;
               
       //7 the right stick
               
        
        glm::mat4 number7TheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 6.0f, 1.0f));
        glm::mat4 number7TheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(.0f, 0.5f, .0f));
        glm::mat4 number7TheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 number7TheRightStickMatrix = number7Matrix * number7TheRightStickTranslateMatrix * number7TheRightStickRotationMatrix * number7TheRightStickScaleMatrix;
        
        
        // ************** End of model matrix definitions *********************
      
        
        
        //**************stage***********************************
        // stage
        glm::mat4 STScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
        glm::mat4 STRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 STTranslateMatrix = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f, 2*gridUnit, -p+ 20 * gridUnit ));
        glm::mat4 STMatrix = worldRotationMatrix * STTranslateMatrix * STRotationMatrix * STScaleMatrix;
        
        
        
        //stage the top stick
        glm::mat4 STTheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(80*gridUnit, 4*gridUnit, 30*gridUnit));
        glm::mat4 STTheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 STTheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glm::mat4 STTheTopStickMatrix = STMatrix * STTheTopStickTranslateMatrix * STTheTopStickRotationMatrix * STTheTopStickScaleMatrix;

        //stage the left stick
             
        glm::mat4 STTheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(10*gridUnit, 4*gridUnit, 10*gridUnit));
        glm::mat4 STTheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 STTheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-35*gridUnit, 0.0f,20*gridUnit));
        glm::mat4 STTheLeftStickMatrix = STMatrix * STTheLeftStickTranslateMatrix * STTheLeftStickRotationMatrix * STTheLeftStickScaleMatrix;
                
    
        //stage the right stick
               
        glm::mat4 STTheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(10*gridUnit, 4*gridUnit, 10*gridUnit));
        glm::mat4 STTheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 STTheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(35*gridUnit, 0.0f, 20*gridUnit));
        glm::mat4 STTheRightStickMatrix = STMatrix * STTheRightStickTranslateMatrix * STTheRightStickRotationMatrix * STTheRightStickScaleMatrix;
        
        
        

        //**************pillars and Screen ***********************************
        
        
        //pillars and Screen
        glm::mat4 PSScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
        glm::mat4 PSRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 PSTranslateMatrix = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f , 4*gridUnit, -p+ 10*gridUnit));
        glm::mat4 PSMatrix = worldRotationMatrix * PSTranslateMatrix * PSRotationMatrix * PSScaleMatrix;
        
        
        //pillars the left stick
             
        glm::mat4 PILTheLeftStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2*gridUnit, 30*gridUnit, 2*gridUnit));
        glm::mat4 PILTheLeftStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 PILTheLeftStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-25*gridUnit, 15*gridUnit, 0.0f));
        glm::mat4 PILTheLeftStickMatrix = PSMatrix * PILTheLeftStickTranslateMatrix * PILTheLeftStickRotationMatrix * PILTheLeftStickScaleMatrix;
                
        
        //pillars the right stick
               
        glm::mat4 PILTheRightStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2*gridUnit, 30*gridUnit, 2*gridUnit));
        glm::mat4 PILTheRightStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 PILTheRightStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(25*gridUnit, 15*gridUnit, 0.0f));
        glm::mat4 PILTheRightStickMatrix = PSMatrix * PILTheRightStickTranslateMatrix * PILTheRightStickRotationMatrix * PILTheRightStickScaleMatrix;
        
        //screen
        glm::mat4 SCTheTopStickScaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(50*gridUnit, 25*gridUnit, 0.1*gridUnit));
        glm::mat4 SCTheTopStickRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 SCTheTopStickTranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 15*gridUnit, 0.0f));
        glm::mat4 SCTheTopStickMatrix = PSMatrix * SCTheTopStickTranslateMatrix * SCTheTopStickRotationMatrix * SCTheTopStickScaleMatrix;
                
        
        
        

        
        //******************************************************
        
        
        // ************** Draw 5 sets of model ****************************
        glActiveTexture(GL_TEXTURE0);
        textureShader.use();
        worldMatrixLocation = glGetUniformLocation(textureShader.program, "worldMatrix");
        GLuint textureLocation = glGetUniformLocation(textureShader.program, "textureSampler");
        // ====== 1st set of models =====
        // Letter J
        glBindTexture(GL_TEXTURE_2D, boxTextureID);
        glUniform1i(textureLocation, 0);

        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &JTheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &JTheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &JTheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &JTheBotStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        //Letter i
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &iTheDotMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &iTheStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
       
        glBindTexture(GL_TEXTURE_2D, metalTextureID);
        glUniform1i(textureLocation, 0);
        //Number 4_1
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4TheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        
        //Number 0_1
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number0TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number0TheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number0TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number0TheBotStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
       
        
        // ====== 2st set of models =====
        // Letter L
        glBindTexture(GL_TEXTURE_2D, boxTextureID);
        glUniform1i(textureLocation, 0);
        
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &LTheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &LTheBotStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        
        
        //Letter E
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &ETheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &ETheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &ETheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &ETheBotStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        glBindTexture(GL_TEXTURE_2D, metalTextureID);
        glUniform1i(textureLocation, 0);
        //Number 4_2
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_2TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_2TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_2TheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        //Number 8
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number8TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number8TheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number8TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number8TheBotStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number8TheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        
        // ====== 3st set of models =====
        
        //Leter Z
        glBindTexture(GL_TEXTURE_2D, boxTextureID);
        glUniform1i(textureLocation, 0);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &ZTheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &ZTheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &ZTheBotStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);

        //Leter N
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &NTheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &NTheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &NTheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);

        //Number 4_3
        glBindTexture(GL_TEXTURE_2D, metalTextureID);
        glUniform1i(textureLocation, 0);
        
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_3TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_3TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_3TheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        //Number 6
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number6TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number6TheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number6TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number6TheBotStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number6TheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);

        
        
        // ====== 4st set of models =====
        
        //Letter Y
        glBindTexture(GL_TEXTURE_2D, boxTextureID);
        glUniform1i(textureLocation, 0);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &YTheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &YTheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &YTheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        //Letter N
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &n_2TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &n_2TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &n_2TheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        glBindTexture(GL_TEXTURE_2D, metalTextureID);
        glUniform1i(textureLocation, 0);
        //Number 4_4
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_4TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_4TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_4TheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        //Number 0_2
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number0_2TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number0_2TheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number0_2TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number0_2TheBotStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        
        // ====== 5st set of models =====
        //Letter F
        glBindTexture(GL_TEXTURE_2D, boxTextureID);
        glUniform1i(textureLocation, 0);
        
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &FTheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &FTheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &FTheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
       
        //Letter G
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &GTheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &GTheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &GTheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &GTheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &GTheBotStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &GTheTopLStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
         
        glBindTexture(GL_TEXTURE_2D, metalTextureID);
        glUniform1i(textureLocation, 0);
        //Number 4_5
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_5TheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_5TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number4_5TheMidStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        //Number 7
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number7TheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &number7TheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
      
        glBindTexture(GL_TEXTURE_2D, leatherTextureID);
        glUniform1i(textureLocation, 0);
        //stage
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &STTheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &STTheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &STTheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        
        
        //pillars and Screen
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &PILTheLeftStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &SCTheTopStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
        glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &PILTheRightStickMatrix[0][0]);
        glDrawArrays(RENDER_MODE, TRIANGLE_OFFSET, TRIANGLE_COUNT);
    
       
        

      
        
        // *************** End of model drawing ***************************
        
        
        // End of frame, used 2 buffers for displaying, this is swapping the buffer
        glBindVertexArray(0);   // why we have to unbind tho?
        glfwSwapBuffers(window);
        
        // ************** handle inputs *************************
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        
        
//        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
//        {
//            glm::vec3 direction(0.0f, 2.0f, 0.0f);
//            eye = eye + direction * movementSpeed * dt;
//            center = center + direction * movementSpeed * dt;
//
//            glm::mat4 viewMatrix = glm::lookAt(eye,  // eye
//                                               center,  // center
//                                               glm::vec3( 0.0f, 1.0f, 0.0f) );// up
//        }
//        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
//        {
//            glm::vec3 direction(0.0f, -2.0f, 0.0f);
//            eye = eye + direction * movementSpeed * dt;
//            center = center + direction * movementSpeed * dt;
//
//            glm::mat4 viewMatrix = glm::lookAt(eye,  // eye
//                                               center,  // center
//                                               glm::vec3( 0.0f, 1.0f, 0.0f) );// up
//
//            GLuint viewMatrixLocation = glGetUniformLocation(textureShader.program, "viewMatrix");
//            glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
//        }

        
        // *******************for world rotations**************
        // UP = Ry
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            worldRotationAngleY += 0.5f;
        }
        // DOWN = R-y
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            worldRotationAngleY -= 0.5f;
        }
        // LEFT = Rx
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            worldRotationAngleZ += 0.5f;
        }
        // RIGHT = R-x
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            worldRotationAngleZ -= 0.5f;
        }
        // *************** end of world rotation **************
        
        // *************** change render mode ***************
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            RENDER_MODE = GL_TRIANGLES;
        }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            RENDER_MODE = GL_LINES;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            RENDER_MODE = GL_POINT;
        }
        // ************* end of changing render mode *************
        
        
        // ********* select manipulating model and predefined camera in front of each model**********
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            ms.currentModel = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            ms.currentModel = 2;

        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            ms.currentModel = 3;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            ms.currentModel = 4;
            // set camera position for model 4
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            ms.currentModel = 5;
        }
        if(glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        {
            // reset camera
            eye = initialEye;
            center = glm::vec3(0.0f, 0.0f, 0.0f);
            // reset world rotation
            worldRotationAngleZ = 0.0f;
            worldRotationAngleY = 0.0f;
            fov = 70.0f;
            ms.resetModels();
        }

        // ************* End of selecting model *****************
        
        
        // ******** Scaling up and down model*************************
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS &&
            (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
        {
            ms.setModelScalingFactor(ms.getModelScalingFactor(ms.currentModel) + 0.2*gridUnit);
        }
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS &&
            (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
        {
            ms.setModelScalingFactor(ms.getModelScalingFactor(ms.currentModel) - 0.2*gridUnit);
        }
        // ************ End of Scaling *********************
        
        // ******** moving the model ***********************
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS &&
            (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
        {

            ms.setModelTranslationPos(ms.getModelTranslationPos(ms.currentModel) + glm::vec3(0.0f, 0.0f, -0.2*gridUnit));
        }
        
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS &&
            (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
        {
            ms.setModelTranslationPos(ms.getModelTranslationPos(ms.currentModel) + glm::vec3(0.0f, 0.0f, 0.2*gridUnit));
        }
        
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
            (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
        {
            ms.setModelTranslationPos(ms.getModelTranslationPos(ms.currentModel) + glm::vec3(-0.2*gridUnit, 0.0f, 0.0f));
        }
        
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
            (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
        {
            ms.setModelTranslationPos(ms.getModelTranslationPos(ms.currentModel) + glm::vec3(0.2*gridUnit, 0.0f, 0.0f));
        }
        // ********* End of model translating ***********
        
        // ********** Rotating the model about y axis *************
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
        {
            ms.setModelRotationAngle(ms.getModelRotationAngle(ms.currentModel) - gridUnit);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
        {
                ms.setModelRotationAngle(ms.getModelRotationAngle(ms.currentModel) + gridUnit);
        }
        // ********** End of model rotation ***************
        //Right click ( use mouse movement in x direction to pan )
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {

            glfwGetCursorPos(window, &mousePosX, &unusedMouse);
            dx = mousePosX - lastMousePosX;
            center -= glm::vec3( dx * 0.05f,0.0f,0.0f);
            lastMousePosX = mousePosX;
            lastMousePosY = unusedMouse;
        }
        else {
            dx = 0.0f;
        }
        //middle button click -> use mouse movement in y direction to tilt.
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            glfwGetCursorPos(window, &unusedMouse, &mousePosY);

            dy = mousePosY - lastMousePosY;
            center += glm::vec3(0.0f,dy * 0.05f,0.0f);
            lastMousePosY = mousePosY;
            lastMousePosX = unusedMouse;
        }
        else {

            dy = 0.0f;
        }

        //Left click -> use mouse movement to move into / out of the scene.
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
          {

            glfwGetCursorPos(window, &unusedMouse, &mousePosY);
            dy = mousePosY - lastMousePosY;
           if(fov >= 1.0f && fov <= 130.0f)
              fov += dy*0.05f;
           if(fov <= 1.0f)
              fov = 1.0f;
           if(fov >= 130.0f)
              fov = 130.0f;
            lastMousePosY = mousePosY;
            lastMousePosX = unusedMouse;
        }
        else {
            dy = 0.0f;
            glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
          }
    
        // update the view matrix
        glm::mat4 viewMatrix = glm::lookAt(eye,  // eye
            center,  // center
            cameraUp);
        setViewMatrix(colorShader.program, viewMatrix);
        setViewMatrix(textureShader.program, viewMatrix);
        
        // update the projection matrix
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), 1024.0f/768.0f, 0.01f, 100.0f);
        setProjectionMatrix(colorShader.program, projectionMatrix);
        setProjectionMatrix(textureShader.program, projectionMatrix);

    }
    glfwTerminate();
    return 0;
}
