#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxOscParameterSync.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
    
        //GUI
        bool showGui;
        ofxPanel gui;
        ofParameterGroup parameters;
    
        ofParameterGroup meshGroup;
        ofParameter<int> meshRadius;
        ofParameter<int> meshRes;
        ofParameter<bool> meshWireframe;
        ofParameter<bool> meshSolid;
    
        ofParameterGroup audioGroup;
        ofParameter<float> volume;
        ofParameter<float> playhead;
        ofParameter<bool> play;
        ofParameter<bool> soundfile;
    
        ofParameterGroup coloursGroup;
        ofParameter<ofFloatColor> background1;
        ofParameter<ofFloatColor> background2;
        ofParameter<ofFloatColor> spherecolour;
        ofParameter<ofFloatColor> wireframecolour;
        ofParameter<bool> background;
    
        ofParameterGroup lightGroup;
        ofParameter<bool> light1on;
        ofParameter<ofFloatColor> light1;
        ofParameter<bool> light2on;
        ofParameter<ofFloatColor> light2;
        ofParameter<bool> light3on;
        ofParameter<ofFloatColor> light3;
    
        ofParameterGroup explodeGroup;
        ofParameter<float> specsmooth;
        ofParameter<bool> explode;
        ofParameter<bool> shaderTog;
        ofParameter<bool> OSCTog;
    
        // FONTS + TEXT
        ofTrueTypeFont arial;
        ofTTFCharacter archar;
        ofTrueTypeFont heiti;
        ofTTFCharacter heitichar;
        void randtext(int len);
        vector<string> randstr;
        string randstrstr;
        string randstrstr1;
        stringstream randst;
    
        //FRAMEBUFFEROBJECT
        ofFbo fbo;
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
        ofShader noiseshader;
        float noiseX;
        float noiseY;
        float noiseZ;
    
        //MESH
        //ofMesh mesh;

        //2D ELEMENTS
        ofImage texture;
        void draw2d();
    
        //SOUNDFILE >>
        ofSoundPlayer sound;
        string path;
        vector<string> title;
    
        //SOUNDSTREAM >>
        void audioIn(float *input, int bufferSize, int nChannels);
        void audioOut(float *input, int bufferSize, int nChannels);
        float soundLevel;
        float rms;
    
        //SPECTRUM VARS
        const int N = 256;
        float spectrum[ 256 ];
        int bandRad = 2;
        int bandVel = 100;
        float frcvar = 1.0;
    
        //OSCRECEIVER >>
        ofxOscParameterSync sync;
    
        //LISTENERS
        void meshRadiusChanged( int & meshRadius );
        void meshResChanged( int & meshRes );
        void playChanged( bool & play );
        void playheadChanged( float & playhead );
        void volumeChanged( float & volume );
        void soundfileChanged( bool & soundfile );
    
    
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
