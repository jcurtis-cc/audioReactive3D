#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
    
        //GUI
        bool showGui;
        ofxPanel gui;
    
        ofxGuiGroup meshGroup;
        ofxIntSlider meshRadius, meshRes;
        ofxFloatSlider deform, deformFreq, extrude, rad;
        ofxToggle meshWireframe, meshSolid;
    
        ofxGuiGroup audioGroup;
        ofxFloatSlider volume, playhead;
        ofxToggle play;
        ofxButton soundfile;
    
        ofxGuiGroup bgGroup;
        ofxToggle background;
    
        ofxGuiGroup lightGroup;
        ofxFloatColorSlider light1, light2, light3;
    
        ofxGuiGroup explodeGroup;
        ofxFloatSlider specsmooth, frcvar;
        ofxToggle explode, shaderTog;
    
        // FONTS + TEXT
        ofTrueTypeFont arial;
        ofTTFCharacter archar;
        void randtext(int len);
        vector<string> randstr;
    
        //FRAMEBUFFEROBJECT
        //ofFbo fbo;
        //ofFbo fbo3d;
    
        //3D PRIMITIVE
        ofSpherePrimitive sphere;
        ofIcoSpherePrimitive icoSphere;
        void draw3d();
        vector<ofMeshFace> triangles;
        vector<ofPoint> vertices0;
        vector<ofPoint> defVertices0;
        vector<ofMeshFace> ogtriangles;
        vector<ofPoint> ogvertices0;
        vector<ofVec3f> ogNormals;
        vector<ofMeshFace> ogTriVerts;
    
        //CAM+LIGHTING+MATERIAL
        ofEasyCam cam;
        ofLight light;
        ofMaterial material;
    
        ofLight pointLight;
        ofLight pointLight2;
        ofLight pointLight3;
    
        //SHADER
        ofShader shader;
    
        //MESH
        //ofMesh mesh;

        //2D ELEMENTS
        ofTexture image;
        void draw2d();
    
        //SOUNDFILE >>
        ofSoundPlayer sound;
        string path;
        //ofFile sffile;
    
        //SOUNDSTREAM >>
        void audioIn(float *input, int bufferSize, int nChannels);
        float soundLevel;
    
        //OSCRECEIVER >>
        ofxOscReceiver oscReceiver;
    
        //LISTENERS
        void meshRadiusChanged( int & meshRadius );
        void meshResChanged( int & meshRes );
        void playChanged( bool & play );
        void playheadChanged( float & playhead );
        void volumeChanged( float & volume );
        void soundfileChanged();
    
    
        //COREFUNC
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};
