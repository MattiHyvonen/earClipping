#include "ofApp.h"

#define TWO_PI 6.28318
#define PI 3.14159

float cross2(const ofVec2f& a, const ofVec2f& b) {    
      return a.x * b.y - a.y * b.x;
}

float sign(float f) {
    if(f>0) return 1;
    else if (f<0) return -1;
    else return 0;
}


float getAngle(ofVec2f v) {
    return atan2(v.y, v.x);    
}

triangle::triangle(ofVec2f A, ofVec2f B, ofVec2f C) {
    points[0] = A;
    points[1] = B;
    points[2] = C;
}


bool triangle::isInside(ofVec2f P) {
    ofVec2f AB = points[1] - points[0]; //0:A  1:B  2:C
    ofVec2f AP = P - points[0];
    ofVec2f BC = points[2] - points[1];
    ofVec2f BP = P - points[1];
    
    if( sign(cross2(AB, AP)) == sign(cross2(BC, BP) ) ) {

        ofVec2f CA = points[0] - points[2];
        ofVec2f CP = P - points[2];

        if(sign(cross2(AB, AP)) == sign(cross2(CA, CP)) ) {
            return true;
        }
    }
    
    return false;
}


//--------------------------------------------------------------
void ofApp::setup(){
    middle = ofVec2f(ofGetWidth()/2, ofGetHeight()/2 );
    P = ofVec2f(0,0);
    
    T.points[0] = ofVec2f(ofRandom(0, ofGetWidth() ), ofRandom(0, ofGetHeight() ) );
    T.points[1] = ofVec2f(ofRandom(0, ofGetWidth() ), ofRandom(0, ofGetHeight() ) );
    T.points[2] = ofVec2f(ofRandom(0, ofGetWidth() ), ofRandom(0, ofGetHeight() ) );
    
    ofSetBackgroundColor(80,80,80);
}

//--------------------------------------------------------------
void ofApp::update(){
}


bool ofApp::vertexIsConvex(int i) {
    //assuming CW winding    
    ofVec2f prev;
    if(i>0) prev = points[i-1];
    else prev = points.back();
    ofVec2f p = points[i];
    ofVec2f next;
    if(i < points.size()-1 ) next = points[i+1];
    else next = points[0];
 
    float prevAngle = getAngle(p - prev);
    float nextAngle = getAngle(next - p);
    
    float ero = nextAngle - prevAngle;
    while(ero < -PI) ero += TWO_PI;
    while(ero > PI) ero -= TWO_PI;
    
    if(ero > 0) return true;
    else return false;
}


void ofApp::triangulate() { 
    std::vector<ofVec2f> points_copy = points;
    triangles.clear();
    
    while(points_copy.size() > 3) {
        bool removed = false;
        //jos kolmion kärki on kupera, ja sen sisällä ei ole verteksejä, se on korva
        for(int i=0; i<points_copy.size() && removed == false; i++) {
            ofVec2f prev, p, next;

            if(i > 0) prev = points_copy[i-1];
            else prev = points_copy.back();

            if(i < points_copy.size()-1) next = points_copy[i+1];
            else next = points_copy[0];

            p = points_copy[i];

            //jos kovera, siirrytään eteenpäin
            if(vertexIsConvex(i) )
                continue;

            //käydään verteksit läpi; jos ne ovat kolmion sisällä, siirrytään eteenpäin        
            triangle t(prev, p, next);
            bool notAnEar = false;
            for(int j=0; j < points_copy.size() && notAnEar == false; j++) {
                //ei tarkasteta kolmion omia pisteitä
                ofVec2f vertex = points_copy[j];
                if(vertex == prev || vertex == p || vertex == next)
                    continue;

                //jos verteksi on sisällä, aloitetaan alusta ja haetaan uusi kolmio
                if(t.isInside(vertex) )
                    notAnEar = true;
            }
            //jos for päättyi niin että sisällä olevaa verteksiä ei ollut, poistetaan p ja lisätään t kolmioihin
            if(notAnEar == false) {
                triangles.push_back(t);
                points_copy.erase(points_copy.begin() + i);
                removed = true;
            }
        }
    }
}


void ofApp::poistaKorva() {
    std::vector<ofVec2f> points_copy = points;
    bool removed = false;
    //jos kolmion kärki on kupera, ja sen sisällä ei ole verteksejä, se on korva
    for(int i=0; i<points_copy.size() && removed == false; i++) {
        ofVec2f prev, p, next;
        
        if(i > 0) prev = points_copy[i-1];
        else prev = points_copy.back();
        
        if(i < points_copy.size()-1) next = points_copy[i+1];
        else next = points_copy[0];
        
        p = points_copy[i];
        
        //jos kovera, siirrytään eteenpäin
        if(vertexIsConvex(i) )
            continue;
        
        //käydään verteksit läpi; jos ne ovat kolmion sisällä, siirrytään eteenpäin        
        triangle t(prev, p, next);
        bool notAnEar = false;
        for(int j=0; j < points_copy.size() && notAnEar == false; j++) {
            //ei tarkasteta kolmion omia pisteitä
            ofVec2f vertex = points_copy[j];
            if(vertex == prev || vertex == p || vertex == next)
                continue;
            
            //jos verteksi on sisällä, aloitetaan alusta ja haetaan uusi kolmio
            if(t.isInside(vertex) )
                notAnEar = true;
        }
        //jos for päättyi niin että sisällä olevaa verteksiä ei ollut, poistetaan p ja lisätään t kolmioihin
        if(notAnEar == false) {
            triangles.push_back(t);
            points_copy.erase(points_copy.begin() + i);
            removed = true;
        }
    }
    points = points_copy;
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //piirrä kursori
    ofSetColor(255,255,255);
    if(!points.empty()) ofDrawLine(points.back(), P);
    
    //piirrä monikulmio
    drawPolygon();
    drawPoints();
    
    //piirrä kolmiot
    ofSetColor(127, 127, 255);
    for(int i=0; i<triangles.size(); i++) {
        drawTriangle(triangles[i]);
    }
    
    ofVec2f v =  P - middle;
    
    if(points.size() > 2) {
        float prevAngle = getAngle(points[points.size()-1] - points[points.size()-2]);
        float nextAngle = getAngle(P - points.back() );
        ofDrawBitmapString( ofToString(prevAngle), 10, 20);
        ofDrawBitmapString( ofToString(nextAngle), 10, 40);
        ofDrawBitmapString( ofToString(nextAngle - prevAngle), 10, 60);
    }
}


void ofApp::drawPoints() {
    for(int i=0; i<points.size(); i++) {
        if(vertexIsConvex(i) ) ofSetColor(0,255, 127);
        else ofSetColor(255,80,0);
        
        ofDrawCircle(points[i], 4);
    }
}


void ofApp::drawPolygon() {

    if(points.size() > 2) {
        for(int i=1; i<points.size(); i++) {
            ofDrawLine(points[i-1], points[i]);
        }
    }
    
    if(!points.empty() )
        ofDrawLine(points.back(), points[0]);
    
}


void ofApp::drawTriangle(triangle t) {
    ofDrawLine(t.points[0], t.points[1] );
    ofDrawLine(t.points[1], t.points[2] );
    ofDrawLine(t.points[2], t.points[0] );
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_BACKSPACE) {
        poistaKorva();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
    P = ofVec2f(x,y);

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    points.push_back(ofVec2f( (float)x, (float)y ) );
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