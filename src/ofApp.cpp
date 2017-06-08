#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
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
    heiti.load("STHeiti.ttc", 20, true, true, true);
    heiti.setLineHeight(20.0f);
    heiti.setLetterSpacing(0.90);
    
    
    //GUI SETUP
    gui.setup( parameters, "AR3Dsettings.xml" );
    
    meshGroup.setName( "meshGroup" );
    meshGroup.add( meshRadius.set ( "meshRadius", 100, 0, 500 ) );
    meshGroup.add( meshRes.set ( "meshRes", 3, 3, 5 ) );
    meshGroup.add( meshWireframe.set ( "meshWireframe", true ) );
    meshGroup.add( meshSolid.set ( "meshSolid", true ) );
    gui.add( meshGroup );

    audioGroup.setName( "audio" );
    audioGroup.add( volume.set ( "volume", 0.8, 0.0, 1.0 ) );
    audioGroup.add( playhead.set ( "playhead", 0.0, 0.0, 1.0 ) );
    audioGroup.add( soundfile.set ( "load_soundfile", false ) );
    audioGroup.add( play.set ( "play", false ) );
    gui.add( audioGroup );
    
    coloursGroup.setName( "colours" );
    coloursGroup.add( background1.set ( "background1", ofColor(0, 0, 0), ofColor( 0, 0 ), ofColor(255, 255)));
    coloursGroup.add( background2.set ( "background2", ofColor(255, 255, 255), ofColor(0, 0 ), ofColor(255, 255)));
    coloursGroup.add( spherecolour.set ( "spherecolour", ofColor(255, 0, 0), ofColor(0, 0 ), ofColor(255, 255)));
    coloursGroup.add( wireframecolour.set ( "wireframecolour", ofColor(255, 0, 0), ofColor(0, 0 ), ofColor(255, 255)));
    gui.add( coloursGroup );
    
    lightGroup.setName( "lighting" );
    lightGroup.add( light1on.set( "light1on", true ) );
    lightGroup.add( light1.set ( "lightone", ofColor( 255, 0, 0 ), ofColor( 0, 0 ), ofColor( 255, 255 )));
    lightGroup.add( light2on.set( "light2on", true ) );
    lightGroup.add( light2.set ( "lighttwo", ofColor( 0, 0, 0 ), ofColor( 0, 0 ), ofColor( 255, 255 )));
    lightGroup.add( light3on.set( "light3on", true ) );
    lightGroup.add( light3.set ( "lightthree", ofColor( 255, 255, 255 ), ofColor( 0, 0 ), ofColor( 255, 255 )));
    gui.add( lightGroup );
    
    explodeGroup.setName( "explode" );
    explodeGroup.add( specsmooth.set ( "specsmooth", 0.9, 0.0, 1.0 ) );
    explodeGroup.add( explode.set ( "explode", true ) );
    explodeGroup.add( shaderTog.set ( "shader", false ) );
    explodeGroup.add( OSCTog.set ( "OSC on/off", false ) );
    gui.add( explodeGroup );
    

    //---------------
    gui.minimizeAll();
    gui.loadFromFile( "AR3Dsettings.xml" );
    showGui = true;
    
    // LISTENERS
    meshRadius.addListener(this, &ofApp::meshRadiusChanged );
    meshRes.addListener(this, &ofApp::meshResChanged );
    play.addListener(this, &ofApp::playChanged );
    playhead.addListener(this, &ofApp::playheadChanged );
    volume.addListener(this, &ofApp::volumeChanged );
    soundfile.addListener(this, &ofApp::soundfileChanged );
    
    
    //2D SETUP
    ofDisableArbTex();
    texture.load("opart1.jpg");
    
    // 3D SETUP
    icoSphere.setRadius(meshRadius);
    icoSphere.setResolution(meshRes);
    ogvertices0 = icoSphere.getMesh().getVertices();
    ogtriangles = icoSphere.getMesh().getUniqueFaces();
    icoSphere.setMode( OF_PRIMITIVE_TRIANGLE_FAN );
    
    // LIGHTING + MATERIAL SETUP
    ofSetSmoothLighting(true);
    material.setShininess( 100 );
    material.setSpecularColor(ofColor(255, 255, 255, 255));
   
    // SHADER SET UP
    shader.load("shadersGL2/shader");
    noiseshader.load("noise");
    
    // FBO SETUP
    fbo.allocate(ofGetScreenWidth(), ofGetScreenHeight());
    
    
    // SOUND SETUP
    sound.setVolume( volume );
    sound.setLoop( true );
    //Set spectrum values to 0
    for (int i=0; i<N; i++){
        spectrum[i] = 0.0f;
    }
    

    
    //SOUNDSTREAMSETUP >>
    soundLevel = 0;
    ofSoundStreamSetup( 1, 1, 44100, 128, 4);
    
    //OSCRECEIVER SETUP >>
    sync.setup((ofParameterGroup&)gui.getParameter(),12346,"localhost",12347);
    //ofSetVerticalSync(true);
    
    //STRINGVECTORSETUP
    title.assign(2, " ");
}

//--------------------------------------------------------------
void ofApp::randtext(int len){
    randstr.clear();
    vector<string> chartable = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","0","1","2","3","4","5","6","7","8","9","0","~","`","!","@","#","$","%","^","&","*","(",")","-","_","+","=","<","?","/","\","};
    
    for( int i=0; i<len; i++ ){
        int rando = (int)ofRandom(-1,chartable.size());
        //int ch = ofMap(ch, 0., rando, 0, chartable.size()) ;
        string cur = chartable[rando];
        randstr.push_back(cur);
    }
    return randstr;
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
void ofApp::audioOut(float *output, int bufferSize, int nChannels){

}


//--------------------------------------------------------------
void ofApp::exit() {
    gui.saveToFile( "AR3Dsettings.xml" );
    ofSoundStreamClose();
    sound.unload();
    sound.stop();
    
}

//--------------------------------------------------------------
void ofApp::playChanged( bool & play ){
    if(sound.isLoaded()){
        if (!play) {
            sound.stop();
        } else sound.play();
    } else {  }
    
}

//--------------------------------------------------------------
void ofApp::soundfileChanged( bool & soundfile ){
    
    sound.stop();
    sound.unload();
    
    ofFileDialogResult result = ofSystemLoadDialog("Load file");
    if(result.bSuccess) {
        path = result.getPath();
        title = ofSplitString(path, "-");
        std::reverse(title.begin(), title.end());
        
        for (int i=0; i<title.size(); i++){
            cout << title[i] << endl;
        }
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
    if(OSCTog){
     sync.update();
    }
    ofSoundUpdate();
    playhead = sound.getPosition();
    
    // SPECTRUM
    
    //Get current spectrum with N bands
    float *val = ofSoundGetSpectrum( N );
    
    double level = 0;
    for (int i=0; i<N; i++) {
        level += spectrum[i] * spectrum[i];
    }
    level = sqrt( level / N );
    

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
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    float time = ofGetElapsedTimef();
    ofFloatColor bg1 = background1;
    ofFloatColor bg2 = background2;
    ofBackgroundGradient(bg1 * spectrum[bandRad], bg2);

    
    
    // NOISE SHADER
    noiseshader.begin();
        noiseX = spectrum[128]*100.0;
        noiseY = spectrum[152]*100.0;
        noiseZ = spectrum[102]*55.0;
        noiseshader.setUniform3f("noisexyz", noiseX, noiseY, noiseZ);
        ofNoFill();
        ofDrawRectangle(-5, spectrum[bandVel]*1000.0, ofGetScreenWidth()+10, spectrum[bandRad]*100.0);
        ofDrawLine(0, ofGetHeight()*spectrum[17], ofGetWidth(), ofGetHeight()*spectrum[17]);
        ofDrawLine(0, ofGetHeight()-ofGetHeight()*spectrum[5], ofGetWidth(), ofGetHeight()-ofGetHeight()*spectrum[5]);
    
        ofDrawRectangle(-5, ofGetHeight()-(spectrum[bandVel]*ofGetHeight()), ofGetScreenWidth()+10, spectrum[bandRad]*100.0);
    
        ofPath curve;
            ofPoint p(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2);
            curve.arc(p, 250, 250, playhead*360, playhead*360+1);
            curve.arcNegative(p, 200, 200, playhead*360+1, playhead*360);
            curve.close();
            curve.setArcResolution(60);
            ofColor c(255, 0,0);
            curve.setFillColor(c);
            curve.setFilled(true);
            curve.draw();

    noiseX = spectrum[128]*0.1;
    noiseY = spectrum[128]*0.1;
    noiseZ = spectrum[102]*0.1;
    ofPath curve2;
    curve2.arc(p, 300, 300, sin(time)*360, sin(time)*360+(spectrum[bandVel]*45));
    curve2.arcNegative(p, 250, 250, sin(time)*360+(spectrum[bandVel]*45), sin(time)*360);
    curve2.close();
    curve2.setArcResolution(60);
    ofColor c2(200, 200,200);
    curve2.setFillColor(c2);
    curve2.setFilled(true);
    //curve2.scale(spectrum[bandVel],spectrum[bandVel]);
    if(sound.isPlaying()){
        curve2.draw();
    }
    
    ofPath curve3;
    curve3.arc(p, 300, 300, 0, spectrum[bandVel]*45);
    curve3.arcNegative(p, 250, 250, spectrum[bandVel]*45, 0);
    curve3.close();
    curve3.setArcResolution(60);
    ofColor c3(50, 50,50);
    curve3.setFillColor(c3);
    curve3.setFilled(true);
    if(sound.isPlaying()){
        curve3.draw();
    }
    
    ofPath curve4;
    curve4.arc(p, 350, 350, sin(time), spectrum[bandVel]*15);
    curve4.arcNegative(p, 300, 300, spectrum[bandVel]*15, sin(time));
    curve4.close();
    curve4.setArcResolution(60);
    ofColor c4(50, 50, 50, 50);
    curve4.setFillColor(c3);
    curve4.setFilled(true);
    if(sound.isPlaying()){
        curve4.draw();
    }
    
    noiseshader.end();

    ofEnableDepthTest();
    ofSetColor( 0, 0, 0 );
    ofFill();
    
    // BEGIN CAMERA
    cam.begin();
    ofRotateY(time*(10*spectrum[bandVel]));
    
    // HUD3D
    string fps = ofToString(ofGetFrameRate(), 0);
    ofSetColor(255,255,255,255);
    //apply font scaling here: DONE
    //can we generate noise characters?
    ofFill();
    heiti.drawStringAsShapes("fps: " + fps, meshRadius+27, meshRadius-meshRadius/1.05);
    
    ofDrawRectangle(meshRadius+20+50*rms, meshRadius-meshRadius/1.1+20, 50*rms, 20);
    
    
    //DRAW RANDOM TEXT
    int charnumbr = spectrum[bandRad]*15.0;
    randtext(charnumbr);
    
    for(int i=0; i < randstr.size();i++){
        ofSetColor(i*10, 0, 0);
        heiti.drawStringAsShapes(randstr[i], meshRadius+27+i*10, meshRadius-meshRadius/1.1+15);
    }
    ofSetColor(255);
    heiti.drawStringAsShapes(title[0], meshRadius+20, meshRadius-meshRadius/1.1+40);
    
    //DRAW MINI BARGRAPH FFT
    for (int i=0; i<N; i++) {
        if ( i == bandRad || i == bandVel ) {
            ofSetColor( 255, 0, 0, 255 ); //Black color
        } else { ofSetColor( 255, 255, 255, 255 ); }
        ofFill();
        ofDrawRectangle( (meshRadius+27+N/2) - i * 0.25, 0, 2, fmod (spectrum[i] * 500, 50) );
        ofDrawRectangle( (meshRadius+27) + i * 0.25, 0, 2, fmod (spectrum[i] * -500, -50) );
    }
    
    
    if(shaderTog){
    // BEGIN SHADER
        shader.begin();
        shader.setUniform1f("bandRad", spectrum[bandRad]/4.0);
        shader.setUniform1f("t", time);
    }
    
    // POINTLIGHTS
    if(light1on)pointLight.enable();
    if(light2on)pointLight2.enable();
    if(light3on)pointLight3.enable();
    
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
    

    
    


    
    ofFloatColor spherecol = spherecolour;

    //ofSetColor( fmod (spectrum[bandRad]*255, 255)+255, 255-spectrum[bandRad]*255, 255-spectrum[bandRad]*255 );
    ofSetColor(spherecol/spectrum[bandRad]*255, 255);
    
    
    
    //FFT EXPLODER
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
        ofFloatColor wireframecol = wireframecolour;
        ofSetColor( wireframecol );
        icoSphere.drawWireframe();

    } else if (meshSolid) {
        material.begin();
        icoSphere.draw();
        material.end();
    } else if (meshWireframe) {
        ofNoFill();
        ofFloatColor wireframecol = wireframecolour;
        ofSetColor( wireframecol );
        icoSphere.drawWireframe();
    }
    
    ofDisableDepthTest();
    ofDisableLighting();

    if(shaderTog){
        shader.end();
    }
    
    cam.end();

    ofRotate(spectrum[bandVel]*2);
    
    
    gui.draw();
    //-----------
    ofSetColor(255);
    stringstream ss;
    ss << "Framerate: " << ofToString(ofGetFrameRate(),0) << "\n";
    
    //ofEnableDepthTest();
    ofSetColor( 0, 0, 0 );
    ofFill();
    
    ofDrawBitmapString("audioReactive3D v1.0", ofGetWidth()-170, ofGetHeight()-60);
    ofDrawBitmapString("comp.proto ARCH1477", ofGetWidth()-170, ofGetHeight()-40);
    ofDrawBitmapString("s3377610 :: may2017", ofGetWidth()-170, ofGetHeight()-20);
    
    
    

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
