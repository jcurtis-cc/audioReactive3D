#include "ofApp.h"

const int N = 256; // Number of bands in the spectrum
float spectrum[ N ]; // Smoothed spectrum values
int bandRad = 2; // Band index in spectrum affecting Rad value
int bandVel = 100; // Band index in spectrum, affecting Vel value

//--------------------------------------------------------------
void ofApp::setup(){
    
    randtext(5);
    
    // OF
    ofSetWindowTitle( "audioReactive3D" );
    ofSetWindowShape( 1920, 1080 );
    ofSetFrameRate( 60 );
    ofBackground( ofColor::black );
    
    // FONTS
    ofTrueTypeFont::setGlobalDpi(72);
    arial.load("arial.ttf", 20, true, true, true);
    arial.setLineHeight(20.0f);
    arial.setLetterSpacing(0.90);
    
    // LISTENERS
    meshRadius.addListener(this, &ofApp::meshRadiusChanged );
    meshRes.addListener(this, &ofApp::meshResChanged );
    play.addListener(this, &ofApp::playChanged );
    playhead.addListener(this, &ofApp::playheadChanged );
    volume.addListener(this, &ofApp::volumeChanged );
    soundfile.addListener(this, &ofApp::soundfileChanged );
    
    //GUI SETUP
    gui.setup( "Parameters", "AR3Dsettings.xml" );
    
    meshGroup.setup( "mesh" );
    meshGroup.add( meshRadius.setup ( "meshRadius", 100, 0, 500 ) );
    meshGroup.add( meshRes.setup ( "meshRes", 1, 0, 5 ) );
    meshGroup.add( deform.setup( "deform", 0.3, 0, 1.5 ) );
    meshGroup.add( deformFreq.setup( "deformFreq", 3, 0, 10 ) );
    meshGroup.add( extrude.setup( "extrude", 1, 0, 1 ) );
    meshGroup.add( rad.setup( "rad", 250, 0, 500 ) );
    meshGroup.add( meshWireframe.setup ( "meshWireframe", true ) );
    meshGroup.add( meshSolid.setup ( "meshSolid", true ) );
    gui.add( &meshGroup );

    audioGroup.setup( "audio" );
    audioGroup.add( volume.setup ( "volume", 0.8, 0.0, 1.0 ) );
    audioGroup.add( playhead.setup ( "playhead", 0.0, 0.0, 1.0 ) );
    audioGroup.add( soundfile.setup ( "load soundfile" ) );
    audioGroup.add( play.setup ( "play", false ) );
    gui.add( &audioGroup );
    
    bgGroup.setup( "background" );
    bgGroup.add( background.setup ( "background", true ) );
    gui.add( &bgGroup );
    
    lightGroup.setup( "lighting " );
    lightGroup.add( light1.setup ( "light1", ofColor( 255, 0, 0 ), ofColor( 0, 0 ), ofColor( 255, 255 )));
    lightGroup.add( light2.setup ( "light2", ofColor( 0, 0, 0 ), ofColor( 0, 0 ), ofColor( 255, 255 )));
    lightGroup.add( light3.setup ( "light3", ofColor( 255, 255, 255 ), ofColor( 0, 0 ), ofColor( 255, 255 )));
    gui.add( &lightGroup );
    
    explodeGroup.setup( "explode" );
    explodeGroup.add( specsmooth.setup ( "specsmooth", 0.9, 0.0, 1.0 ) );
    explodeGroup.add( frcvar.setup ( "frcvar", 0.0, -5.0, 5. ) );
    explodeGroup.add( explode.setup ( "explode", false ) );
    explodeGroup.add( shaderTog.setup ( "shader", false ) );
    gui.add( & explodeGroup );
    

    //---------------
    gui.minimizeAll();
    gui.loadFromFile( "AR3Dsettings.xml" );
    showGui = true;
    
    //2D SETUP
    ofLoadImage( image, "screenshot27.png" );
    
    // 3D SETUP
    icoSphere.setRadius(meshRadius);
    icoSphere.setResolution(meshRes);
    //icoSphere.set( 25, );
    //icoSphere.setMode( OF_PRIMITIVE_TRIANGLE_STRIP );
    
    // LIGHTING + MATERIAL SETUP
    ofSetSmoothLighting(true);
    
    // shininess is a value between 0 - 128, 128 being the most shiny //
    material.setShininess( 100 );
    // the light highlight of the material //
    material.setSpecularColor(ofColor(255, 255, 255, 255));
   
    // SHADER SET UP
    #ifdef TARGET_OPENGLES
    shader.load("shadersES2/shader");
    cout << "shaderES2" << endl;
    #else
    if(ofIsGLProgrammableRenderer()){
        shader.load("shadersGL3/shader");
        cout << "shaderGL3" << endl;
    }else{
        shader.load("shadersGL2/shader");
        cout << "shaderGL2" << endl;
    }
    #endif
    
    
    // SOUND SETUP
    //sound.load( "CalqaBarbarix.mp3" );
    sound.setVolume( volume );
    sound.setLoop( true );
    
    
    //Set spectrum values to 0
    for (int i=0; i<N; i++){
        spectrum[i] = 0.0f;
    }
    
    //SOUNDSTREAMSETUP >>
    soundLevel = 0;
    ofSoundStreamSetup( 0, 1, 44100, 128, 4);
    
    //OSCRECEIVER SETUP >>
    oscReceiver.setup( 12346 );
}

//--------------------------------------------------------------
void ofApp::randtext(int len){
    vector<string> chartable = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","0","1","2","3","4","5","6","7","8","9","0","~","`","!","@","#","$","%","^","&","*","(",")","-","_","+","=","<","?","/","\","};
    
    for( int i=0; i<len; i++ ){
        int rando = ofRandom(0, 1.);
        int ch = ofMap(ch, 0., rando, 0, chartable.size()) ;
        cout << chartable.size() << endl;
        cout << ch;
        string cur = chartable[i];
        randstr.push_back(cur);
    }
    for ( int i=0; i<randstr.size(); i++){
        //cout << randstr[i] << endl;
    }
}



//--------------------------------------------------------------
void ofApp::audioIn(float *input, int bufferSize, int nChannels) {
    double v = 0;
    for (int i =0; i<bufferSize; i++) {
        v += input[i] * input[i];
    }
    v = sqrt( v/ bufferSize );
    soundLevel = v;
}

//--------------------------------------------------------------
void ofApp::exit() {
    gui.saveToFile( "AR3Dsettings.xml" );
}

//--------------------------------------------------------------
void ofApp::playChanged(bool & play){
    if (!play) {
        sound.stop();
    } else sound.play();
    
}

//--------------------------------------------------------------
void ofApp::soundfileChanged(){
    
    sound.stop();
    sound.unload();
    
    ofFileDialogResult result = ofSystemLoadDialog("Load file");
    if(result.bSuccess) {
        path = result.getPath();
        // load your file at `path`
        sound.load(path);
    }
}

//--------------------------------------------------------------
void ofApp::playheadChanged(float & playhead){
    sound.setPosition( playhead );
    
}

//--------------------------------------------------------------
void ofApp::volumeChanged(float & volume){
    sound.setVolume( volume );
    
}

//--------------------------------------------------------------
void ofApp::meshRadiusChanged(int & meshRadius){
    icoSphere.setRadius(meshRadius);
    
    //OR mesh.copy??
    ogvertices0 = icoSphere.getMesh().getVertices();
    ogtriangles = icoSphere.getMesh().getUniqueFaces();
    for(size_t i = 0; i < triangles.size(); i++ ) {
        for(size_t j = 0; j < 3; j++){
            //ogTriVerts[i] = ogtriangles[i].getVertex(j);
        }
    }
}

//--------------------------------------------------------------
void ofApp::meshResChanged(int & meshRes){
    icoSphere.setResolution(meshRes);
    
    ogvertices0 = icoSphere.getMesh().getVertices();
    ogtriangles = icoSphere.getMesh().getUniqueFaces();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSoundUpdate();
    playhead = sound.getPosition();
    
    // RMS (128 bands) >>
    //float *spectrum = ofSoundGetSpectrum(128);

    
    //MAP RMS >>
    //float newRad = ofMap( level, 0, 1, 100, 200, true ); // << 'true' clips to output range!
    //rad = rad + 0.2 * (newRad-rad); // << exponential smoothing (0.1 = smoothing factor )
    //extrude = level;
    
    
    // SPECTRUM
    
    //Get current spectrum with N bands
    float *val = ofSoundGetSpectrum( N );
    
    double level = 0;
    for (int i=0; i<N; i++) {
        level += spectrum[i] * spectrum[i];
    }
    level = sqrt( level / N );
    
    level += soundLevel; // increases level by soundLevel (mic/input) ie: reacts to both
    
    //Update smoothed spectrum by slowly decreasing its values and getting maximum with val
    //So we will have slowly falling peaks in spectrum
    for (int i=0; i<N; i++) {
        spectrum[i] *= specsmooth; // 0.97
        spectrum[i] = max( spectrum[i], val[i] );
    }
    
    
    


    
}

//--------------------------------------------------------------
void ofApp::draw3d() {

}

//--------------------------------------------------------------
void ofApp::draw2d(){
    //ofBackground( Background );
    if( background ) image.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackgroundGradient(55 * spectrum[bandRad], 255);
    ofEnableDepthTest();
    float time = ofGetElapsedTimef();
    
    ofSetColor( 0, 0, 0 );
    ofFill();

    ofDrawBitmapString("audioReactive3D v1.0", ofGetWidth()-170, ofGetHeight()-60);
    ofDrawBitmapString("comp.proto ARCH1477", ofGetWidth()-170, ofGetHeight()-40);
    ofDrawBitmapString("s3377610 :: may2017", ofGetWidth()-170, ofGetHeight()-20);

    
    
    
    draw2d();
    //draw3d();
    

    // BEGIN CAMERA
    cam.begin();
    ofRotateY(time*(10*spectrum[bandVel]));
    // HUD3D
    string fps = ofToString(ofGetFrameRate(), 0);
    ofSetColor(255,255,255,255);
    //apply font scaling here: DONE
    //can we generate noise characters?
    ofFill();
    arial.drawStringAsShapes("fps: " + fps, meshRadius+15, meshRadius-meshRadius/1.1);
    arial.drawStringAsShapes(ofToString(triangles.size()), meshRadius+15, meshRadius-meshRadius/1.1+20);
    float skip = triangles.size()/(float)N;
    arial.drawStringAsShapes(ofToString(skip), meshRadius+15, meshRadius-meshRadius/1.1+40);
    
    
    for (int i=0; i<N; i++) {
        //Draw bandRad and bandVel by black color,
        //and other by gray color
        if ( i == bandRad || i == bandVel ) {
            ofSetColor( 255, 0, 0, 255 ); //Black color
        } else { ofSetColor( 255, 255, 255, 255 ); }
        ofFill();
        ofDrawRectangle( (meshRadius+17+N/3) - i * 0.25, 0, 1, fmod (spectrum[i] * 500, 50) );
        ofDrawRectangle( (meshRadius+17) + i * 0.25, 0, 1, fmod (spectrum[i] * -500, -50) );
    }
    
   
    
  
    
    if(shaderTog){
    // BEGIN SHADER
        shader.begin();
        shader.setUniform1f("bandRad", spectrum[bandRad]/4.0);
        shader.setUniform1f("t", time);
    }
    
    // POINTLIGHTS
    pointLight.enable();
    pointLight2.enable();
    pointLight3.enable();
    
    pointLight.setDiffuseColor( light1 );
    pointLight.setSpecularColor( light1 );
    
    pointLight2.setDiffuseColor( light2 );
    pointLight2.setSpecularColor( 0 );
    
    pointLight3.setDiffuseColor( light3 );
    pointLight3.setSpecularColor( 0 );
    
    
    pointLight.setPosition((ofGetWidth()*.5)+ cos(ofGetElapsedTimef()*.5)*(ofGetWidth()*.3), ofGetHeight()/2, 500);
    pointLight2.setPosition((ofGetWidth()*.5)+ cos(ofGetElapsedTimef()*.15)*(ofGetWidth()*.3),
                            ofGetHeight()*.5 + sin(ofGetElapsedTimef()*.7)*(ofGetHeight()), -300);
    
    pointLight3.setPosition(
                            cos(ofGetElapsedTimef()*1.5) * ofGetWidth()*.5,
                            sin(ofGetElapsedTimef()*1.5f) * ofGetWidth()*.5,
                            cos(ofGetElapsedTimef()*.2) * ofGetWidth()
                            );
    

    
    


    vertices0 = icoSphere.getMesh().getVertices();
    
//    //MESH DEFORM >>
//    if(deformTog) {
//        vector<ofPoint> &vertices = icoSphere.getMesh().getVertices();
//        for (int i=0; i<vertices.size(); i++) {
//            ofPoint v = vertices0[i];
//            v.normalize();
//            float sx = sin( v.x * deformFreq );
//            float sy = sin( v.y * deformFreq );
//            float sz = sin( v.z * deformFreq );
//            v.x += v.x + sin(deform);
//            v.y += v.y + sin(deform);
//            v.z += v.z + sin(deform);
//            v *= meshRadius;
//            vertices[i] = v;
//        }
//        ofPixels pixels;
//        fbo.readToPixels(pixels);
//    
//        for (int i=0; i<vertices.size(); i++) {
//            ofVec2f t = icoSphere.getMesh().getTexCoords()[i];
//            t.x = ofClamp( t.x, 0, pixels.getWidth()-1 );
//            t.y = ofClamp( t.y, 0, pixels.getHeight()-1 );
//            float br = pixels.getColor(t.x, t.y).getBrightness();
//            vertices[i] *= 1 + br / 255.0 * extrude;
//        }
//    }
    
    
//    if (explode){
//        vertices0 = icoSphere.getMesh().getVertices();
//        triangles = icoSphere.getMesh().getUniqueFaces(); // << get triangles from sphere mesh
//
//        float angle = ofGetElapsedTimef()*1.4;
//        //float strength = (sin( angle+.25 )) * .5f * 5.f;
//        ofVec3f faceNormal;
//        for(size_t i = 0; i < triangles.size(); i++ ) {
//            // store the face normal here.
//            // we change the vertices, which makes the face normal change
//            // every time that we call getFaceNormal //
//            float frc = ofSignedNoise(angle* (float)i * .1, angle*.05) * multipl;
//
    
                //faceNormal = triangles[i].getFaceNormal();
//            for(int j = 0; j < 3; j++ ) {
//                triangles[i].setVertex( j, triangles[i].getVertex(j) + faceNormal * frc);
//            }
//         }
//        icoSphere.getMesh().setFromTriangles( triangles );
//    }

    ofSetColor( fmod (spectrum[bandRad]*255, 255)+255, 255-spectrum[bandRad]*255, 255-spectrum[bandRad]*255 );
    
    
    if (explode){
            vertices0 = icoSphere.getMesh().getVertices();
            triangles = icoSphere.getMesh().getUniqueFaces();
            ogNormals = icoSphere.getMesh().getNormals();

            ofVec3f faceNormal;
            int skipp = triangles.size()/N;
    
            for(size_t i = 0; i < triangles.size(); i = i+skipp ) {

                    faceNormal = triangles[i].getFaceNormal();
             
                if (spectrum[i/skipp] > 0){
                    frcvar = ofMap(spectrum[i/skipp], 0., 1,  0, -299.5, true);
                } else frcvar = frcvar * 0.5;
                    float frc = frcvar;
                    for(int j = 0; j < 3; j++ ) {
                        triangles[i].setVertex( j, ogtriangles[i].getVertex(j) + faceNormal * frc);
                        
                    }
            }

        icoSphere.getMesh().setFromTriangles( triangles );
    
    }
    

    
    

    if (meshWireframe && meshSolid) {
        material.begin();
        icoSphere.draw();
        material.end();
        ofDisableLighting();
        ofNoFill();
        ofSetColor( 0, 0, 0, 255);
        icoSphere.drawWireframe();

    } else if (meshSolid) {
        material.begin();
        icoSphere.draw();
        material.end();
    } else if (meshWireframe) {
        ofNoFill();
        ofSetColor( 255, 255, 255, 255);
        icoSphere.drawWireframe();
    }
    
    //AR_ROTATE

    
    
    
    ofDisableDepthTest();
    ofDisableLighting();

    if(shaderTog){
        shader.end();
    }
    
    cam.end();
    
    ofRotate(spectrum[bandVel]*20);
    
    gui.draw();
    //-----------
    ofSetColor(255);
    stringstream ss;
    ss << "Framerate: " << ofToString(ofGetFrameRate(),0) << "\n";
    
    //ofDrawBitmapString(ss.str().c_str(), 20, 20);
    ofDrawBitmapString(path, 20, ofGetHeight()-20);
    
    

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
